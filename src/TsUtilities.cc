#include "public/TsUtilities.h"
#include "public/Ts_IEC13818-1.h"
#include "Logging.h"

#include <fstream>

/// 3rd-party
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

namespace tsutil
{
const plog::Severity TsUtilities::DEFAULT_LOG_LEVEL = plog::debug;
const std::string TsUtilities::LOGFILE_NAME = "mpeg2ts_log.csv";
int TsUtilities::LOGFILE_MAXSIZE = 100 * 1024;
int TsUtilities::LOGFILE_MAXNUMBEROF = 10;

TsUtilities::TsUtilities()
    : mAddedPmts{ false }
{
}

void TsUtilities::initLogging()
{
    static plog::RollingFileAppender<plog::CsvFormatter> fileAppender(LOGFILE_NAME.c_str(), LOGFILE_MAXSIZE, LOGFILE_MAXNUMBEROF); // Create the 1st appender.
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender; // Create the 2nd appender.
    plog::init(DEFAULT_LOG_LEVEL, &fileAppender).addAppender(&consoleAppender); // Initialize the logger with the both appenders.
    plog::init<FileLog>(DEFAULT_LOG_LEVEL, &fileAppender); // Initialize the 2nd logger instance.
}


void TsUtilities::initParse()
{
    initLogging();
    mPmtPids.clear();  // Restart
    mPrevPat = {};
    mPmts.clear();
    mEsPids.clear();
    mAddedPmts = false;
    // Register PAT callback
    mDemuxer.addPsiPid(TS_PACKET_PID_PAT, std::bind(&PATCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), (void*) this);
}

void TsUtilities::registerPmtCallback()
{
    if (!mAddedPmts && !mPmtPids.empty())
    {
        for (auto pid : mPmtPids)
        {
            //LOGD << "Adding PSI PID for parsing: " << pid;
            mDemuxer.addPsiPid(pid, std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), (void*) this);
        }
        mAddedPmts = true;
    }
}

void TsUtilities::registerPesCallback()
{
    for (auto pid : mEsPids)
    {
        LOGD << "Adding PES PID for parsing: " << pid;
        mDemuxer.addPesPid(pid, std::bind(&PESCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), (void*) this);
    }
}


bool TsUtilities::parseTransportFile(const std::string& file)
{
    initParse();
    std::string line;
    std::ifstream tsFile(file, std::ifstream::binary);

    if (!tsFile) {
        return false;
    }

    LOGD << "Parsing tsfile:" << file << std::endl;

    while (!tsFile.eof())
    {
        uint8_t packet[188];
        tsFile.read((char*)packet, 188); // TODO check sync byte
        mDemuxer.demux(packet);
    }
    tsFile.close();

    return true;
}

bool TsUtilities::parseTransportUdpStream(const IpAddress &ip, const Port &p)
{
    return true;
}


bool TsUtilities::parseTransportStreamData(const uint8_t* data, std::size_t size)
{
    //_addedPmts = false;
    initParse();

    // If empty data, just return
    if (data == NULL || data == nullptr)
    {
        LOGE << "No data to parse...";
        return false;
    }

    uint64_t count = 0;
    uint64_t readIndex = 0;



    if ((data[0] != TS_PACKET_SYNC_BYTE) || (size <= 0)) // TODO support maxsize?
    {
        LOGE << "ERROR: 1'st byte not in sync!!!" << std::endl;
        return false;
    }

    while (readIndex < size)
    {
        mDemuxer.demux(data + readIndex);
        readIndex += TS_PACKET_SIZE;
        count++;
    }
    LOGD << "Found " << count << " ts packets." << std::endl;

    return true;
}


void TsUtilities::PATCallback(PsiTable* table, uint16_t pid, void* hdl)
{
    auto instance = reinterpret_cast<TsUtilities*>(hdl); // TODO try/catch
    LOGV_(FileLog) << "PATCallback pid:" << pid;
    PatTable* pat;
    try
    {
        pat = dynamic_cast<PatTable*>(table);
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
            if (program.type == ProgramType::PMT)
            {
                instance->mPmtPids.push_back(program.program_map_PID);
            }
        }
    }

    instance->registerPmtCallback();

    // TODO: add writing of table
}

PatTable TsUtilities::getPatTable() const
{
    return mPrevPat;
}

std::vector<uint16_t> TsUtilities::getPmtPids() const
{
    return mPmtPids;
}

void TsUtilities::PMTCallback(PsiTable* table, uint16_t pid, void* hdl)
{
    auto instance = reinterpret_cast<TsUtilities*>(hdl);

    LOGV_(FileLog) << "PMTCallback... pid:" << pid;
    PmtTable* pmt;

    try
    {
        pmt = dynamic_cast<PmtTable*>(table);
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

    LOGD << "Adding PMT to list..." << std::endl;
    instance->mPmts[pid] = *pmt;
    
    for (auto& stream : pmt->streams)
    {
        LOGD_(FileLog) << "Add ES PID: " << stream.elementary_PID << std::endl;
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

std::map<uint16_t, PmtTable> TsUtilities::getPmtTables() const
{
    return mPmts;
}

std::vector<uint16_t> TsUtilities::getEsPids() const
{
    return mEsPids;
}

void TsUtilities::PESCallback(const PesPacket& pes, uint16_t pid, void* hdl)
{
    auto instance = reinterpret_cast<TsUtilities*>(hdl);

    //LOGV << "PES ENDING at Ts packet " << instance->mDemuxer.getTsStatistics().mTsPacketCounter
    //     << " (" << pid << ")\n";
    //LOGV << pes << std::endl;

    LOGV << "Adding PES to list..." << std::endl;
    instance->mPesPackets[pid].push_back(pes);
    /*
    // @TODO add "if parse pid" option to cmd line
    {
        for (auto& pmtPid : instance->mPmtPids)
        {
            if (instance->mPmts.find(pmtPid) != instance->mPmts.end())
            {
                auto it = std::find_if(instance->mPmts[pmtPid].streams.begin(), instance->mPmts[pmtPid].streams.end(), [&](StreamTypeHeader& stream) {return stream.elementary_PID == pid; });
                if (it != instance->mPmts[pmtPid].streams.end())
                {
                    try
                    {
                        (*g_EsParsers.at(it->stream_type))(&pes.mPesBuffer[pes.elementary_data_offset], pes.mPesBuffer.size() - pes.elementary_data_offset);
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


std::map<uint16_t, std::vector<PesPacket>> TsUtilities::getPesPackets() const
{
    return mPesPackets;
}



} // namespace tsutil
