#include "JsonSettings.h"
#include "Logging.h"
#include <public/TsUtilities.h>
#include <public/Ts_IEC13818-1.h>

#include <fstream>

/// 3rd-party
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>

namespace tsutil
{

// Constants
const LogLevel TsUtilities::DEFAULT_LOG_LEVEL = LogLevel::DEBUG;
const std::string TsUtilities::LOGFILE_NAME = "mpeg2ts_log.csv";
const int TsUtilities::LOGFILE_MAXSIZE = 100 * 1024;
const int TsUtilities::LOGFILE_MAXNUMBEROF = 10;

TsUtilities::TsUtilities()
: mAddedPmts{ false }
{
}

void TsUtilities::initLogging() const
{
    Settings settings;
    std::string logFile = "settings.json";
    bool success = false;
    LoadException openException;
    try
    {
        success = settings.loadFile(logFile); // Must be at same location as dll/so
    }
    catch (LoadException& e)
    {
        openException = e;
    }
    catch (std::exception& e)
    {
        openException = LoadException(e);
    }
    catch (...)
    {
        std::string errorMsg = "Got unknown exception when loading file: " + logFile;
        openException = LoadException(errorMsg);
    }

    plog::Severity logLevel;
    std::string logFileName;
    int maxSize;
    int maxNumberOf;

    if (success)
    {
        const std::string strLogLevel = settings.getLogLevel();

        if (strLogLevel == "VERBOSE")
            logLevel = plog::Severity::verbose;
        else if (strLogLevel == "DEBUG")
            logLevel = plog::Severity::debug;
        else if (strLogLevel == "INFO")
            logLevel = plog::Severity::info;
        else if (strLogLevel == "WARNING")
            logLevel = plog::Severity::warning;
        else if (strLogLevel == "ERROR")
            logLevel = plog::Severity::error;
        else if (strLogLevel == "FATAL")
            logLevel = plog::Severity::fatal;
        else if (strLogLevel == "NONE")
            logLevel = plog::Severity::none;
        else // Default
            logLevel = plog::Severity::debug;

        maxSize = settings.getLogFileMaxSize();
        maxNumberOf = settings.getLogFileMaxNumberOf();
        logFileName = settings.getLogFileName();
    }
    else
    {
        logLevel = plog::Severity::debug; // TODO ugly fix, convert from LogLevel...
        logFileName = LOGFILE_NAME;
        maxSize = LOGFILE_MAXSIZE;
        maxNumberOf = LOGFILE_MAXNUMBEROF;
    }

    static plog::RollingFileAppender<plog::CsvFormatter> fileAppender(logFileName.c_str(), maxSize, maxNumberOf); // Create the 1st appender.
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;  // Create the 2nd appender.
    plog::init(logLevel, &fileAppender).addAppender(&consoleAppender); // Initialize the logger with
                                                                       // the both appenders.
    plog::init<FileLog>(logLevel, &fileAppender); // Initialize the 2nd logger instance.

    if (strlen(openException.what()) > 0)
    {
        LOGE << "Got exception when opening file: " << logFile
             << ", with exception: " << openException.what();
    }
}


void TsUtilities::initParse()
{
    initLogging();
    mPmtPids.clear(); // Restart
    mPrevPat = {};
    mPmts.clear();
    mEsPids.clear();
    mAddedPmts = false;
    // Register PAT callback
    mDemuxer.addPsiPid(mpeg2ts::TS_PACKET_PID_PAT,
                       std::bind(&PATCallback, std::placeholders::_1, std::placeholders::_2,
                                 std::placeholders::_3, std::placeholders::_4),
                       reinterpret_cast<void*>(this));
}

void TsUtilities::registerPmtCallback()
{
    if (!mAddedPmts && !mPmtPids.empty())
    {
        for (auto pid : mPmtPids)
        {
            // LOGD << "Adding PSI PID for parsing: " << pid;
            mDemuxer.addPsiPid(pid,
                               std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2,
                                         std::placeholders::_3, std::placeholders::_4),
                               reinterpret_cast<void*>(this));
        }
        mAddedPmts = true;
    }
}

void TsUtilities::registerPesCallback()
{
    for (auto pid : mEsPids)
    {
        LOGD << "Adding PES PID for parsing: " << pid;
        mDemuxer.addPesPid(pid,
                           std::bind(&PESCallback, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3, std::placeholders::_4),
                           reinterpret_cast<void*>(this));
    }
}


bool TsUtilities::parseTransportFile(const std::string& file)
{
    initParse();
    std::ifstream tsFile(file, std::ifstream::binary);

    if (!tsFile)
    {
        return false;
    }

    LOGD << "Parsing tsfile:" << file;

    while (!tsFile.eof())
    {
        uint8_t packet[188];
        tsFile.read(reinterpret_cast<char*>(packet), 188); // TODO check sync byte
        mDemuxer.demux(packet);
    }
    tsFile.close();

    return true;
}

bool TsUtilities::parseTransportUdpStream(const IpAddress& /* ip*/, const Port& /* p*/)
{
    return true;
}


bool TsUtilities::parseTransportStreamData(const uint8_t* data, std::size_t size)
{
    initParse();

    // If empty data, just return
    if ((data == NULL) || (data == nullptr))
    {
        LOGE << "No data to parse...";
        return false;
    }

    uint64_t count = 0;
    uint64_t readIndex = 0;


    if ((data[0] != mpeg2ts::TS_PACKET_SYNC_BYTE) || (size <= 0)) // TODO support maxsize?
    {
        LOGE << "ERROR: 1'st byte not in sync!!!";
        return false;
    }

    while (readIndex < size)
    {
        mDemuxer.demux(data + readIndex);
        readIndex += mpeg2ts::TS_PACKET_SIZE;
        count++;
    }
    LOGD << "Found " << count << " ts packets.";

    return true;
}


void TsUtilities::PATCallback(const mpeg2ts::ByteVector& /* rawPes*/, mpeg2ts::PsiTable* table, uint16_t pid, void* hdl)
{
    auto instance = reinterpret_cast<TsUtilities*>(hdl); // TODO try/catch
    LOGV_(FileLog) << "PATCallback pid:" << pid;
    mpeg2ts::PatTable* pat;
    try
    {
        pat = dynamic_cast<mpeg2ts::PatTable*>(table);
    }
    catch (std::exception& ex)
    {
        LOGE_(FileLog) << "ERROR: dynamic_cast ex: " << ex.what();
        return;
    }

    if (pat == NULL)
    {
        LOGE_(FileLog) << "ERROR: This should not happen. You have some corrupt stream!!!";
        return;
    }

    // Do nothing if same PAT
    if (instance->mPrevPat == *pat)
    {
        LOGV << "Got same PAT...";
        return;
    }
    instance->mPrevPat = *pat;


    // Check if MPTS or SPTS
    int numPrograms = pat->programs.size();
    if (numPrograms == 0)
    {
        LOGD_(FileLog) << "No programs found in PAT, exiting...";
        exit(EXIT_SUCCESS);
    }
    else if (numPrograms == 1) // SPTS
    {
        LOGD << "Found Single Program Transport Stream (SPTS).";
        instance->mPmtPids.push_back(pat->programs[0].program_map_PID);
    }
    else if (numPrograms >= 1) // MPTS
    {
        LOGD << "Found Multiple Program Transport Stream (MPTS).";
        for (auto program : pat->programs)
        {
            if (program.type == mpeg2ts::ProgramType::PMT)
            {
                instance->mPmtPids.push_back(program.program_map_PID);
            }
        }
    }

    instance->registerPmtCallback();

    // TODO: add writing of table
}

mpeg2ts::PatTable TsUtilities::getPatTable() const
{
    return mPrevPat;
}

std::vector<uint16_t> TsUtilities::getPmtPids() const
{
    return mPmtPids;
}

void TsUtilities::PMTCallback(const mpeg2ts::ByteVector& /* rawPes*/, mpeg2ts::PsiTable* table, uint16_t pid, void* hdl)
{
    auto instance = reinterpret_cast<TsUtilities*>(hdl);

    LOGV_(FileLog) << "PMTCallback... pid:" << pid;
    mpeg2ts::PmtTable* pmt;

    try
    {
        pmt = dynamic_cast<mpeg2ts::PmtTable*>(table);
    }
    catch (std::exception& ex)
    {
        LOGE_(FileLog) << "ERROR: dynamic_cast ex: " << ex.what();
        return;
    }

    if (pmt == NULL)
    {
        LOGE_(FileLog) << "ERROR: This should not happen. You have some corrupt stream!!!";
        return;
    }


    // Do nothing if same PMT
    if (instance->mPmts.find(pid) != instance->mPmts.end())
    {
        LOGV_(FileLog) << "Got same PMT...";
        return;
    }

    LOGD << "Adding PMT to list...";
    instance->mPmts[pid] = *pmt;

    for (auto& stream : pmt->streams)
    {
        LOGD_(FileLog) << "Add ES PID: " << stream.elementary_PID;
        instance->mEsPids.push_back(stream.elementary_PID);
    }
    /*
    if (pmt->PCR_PID != 0)
    {
        if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), pmt->PCR_PID) ||
            std::count(g_Options["write"].begin(), g_Options["write"].end(), pmt->PCR_PID))
        {
            //LOGD << "Add PCR PID: " << pmt->PCR_PID << std::endl;
            instance->mEsPids.push_back(pmt->PCR_PID);
        }
    }*/

    instance->registerPesCallback();
}

std::map<uint16_t, mpeg2ts::PmtTable> TsUtilities::getPmtTables() const
{
    return mPmts;
}

std::vector<uint16_t> TsUtilities::getEsPids() const
{
    return mEsPids;
}

void TsUtilities::PESCallback(const mpeg2ts::ByteVector& /* rawPes*/, const mpeg2ts::PesPacket& pes, uint16_t pid, void* hdl)
{
    auto instance = reinterpret_cast<TsUtilities*>(hdl);

    // LOGV << "PES ENDING at Ts packet " << instance->mDemuxer.getTsStatistics().mTsPacketCounter
    //     << " (" << pid << ")\n";
    // LOGV << pes << std::endl;

    LOGV << "Adding PES to list...";
    instance->mPesPackets[pid].push_back(pes);
    /*
    // @TODO add "if parse pid" option to cmd line
    {
        for (auto& pmtPid : instance->mPmtPids)
        {
            if (instance->mPmts.find(pmtPid) != instance->mPmts.end())
            {
                auto it = std::find_if(instance->mPmts[pmtPid].streams.begin(),
    instance->mPmts[pmtPid].streams.end(), [&](StreamTypeHeader& stream) {return
    stream.elementary_PID == pid; }); if (it != instance->mPmts[pmtPid].streams.end())
                {
                    try
                    {
                        (*g_EsParsers.at(it->stream_type))(&pes.mPesBuffer[pes.elementary_data_offset],
    pes.mPesBuffer.size() - pes.elementary_data_offset);
                    }
                    catch (const std::out_of_range&) {
                        LOGD << "No parser for stream type " << StreamTypeToString[it->stream_type];
                    }
                }
            }
        }
    }*/


    //    LOGD << "Write " << "PES" << ": " << pes.mPesBuffer.size() - writeOffset
    //       << " bytes, pid: " << pid << std::endl;
}


std::map<uint16_t, std::vector<mpeg2ts::PesPacket>> TsUtilities::getPesPackets() const
{
    return mPesPackets;
}

mpeg2ts::PidStatisticsMap TsUtilities::getPidStatistics() const
{
    return mDemuxer.getPidStatistics();
}


} // namespace tsutil
