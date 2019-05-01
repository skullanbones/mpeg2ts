

#include <exception>		                     // for exception                            
#include <stdlib.h>                              // for exit, EXIT_SUCCESS                                                                                                                                                                                    
#include <string.h>                              // for strlen                                                   
#include <algorithm>                             // for copy, max, find_if
#include <cstdint>                               // for uint8_t, uint16_t                                                                                      
#include <fstream>                               // for basic_ostream::opera...                                                                                                                                             
#include <stdexcept>                             // for out_of_range                                                                         

/// 3rd-party
#include "plog/Appenders/ConsoleAppender.h"      // for ConsoleAppender                                                                                   
#include "plog/Log.h"                            // for LOGD, LOGE, LOGE_
#include "plog/Appenders/RollingFileAppender.h"  // for RollingFileAppender
#include "plog/Formatters/CsvFormatter.h"        // for CsvFormatter
#include "plog/Formatters/TxtFormatter.h"        // for TxtFormatter
#include "plog/Init.h"                           // for init
#include "plog/Logger.h"                         // for Logger
#include "plog/Record.h"                         // for Record
#include "plog/Severity.h"                       // for Severity, debug, error

/// Project files
#include "TsUtilities.h"
#include "JsonSettings.h"                        // for LoadException, Settings                   
#include "Logging.h"                             // for FileLog
#include "Ts_IEC13818-1.h"                       // for Program, Program::(a...
#include "h264/H264Parser.h"                     // for EsInfoH264, H264EsPa...
#include "mpeg2vid/Mpeg2VideoParser.h"           // for EsInfoMpeg2, Mpeg2Vi...


namespace tsutil
{

TsUtilities::~TsUtilities()
{
}

// Constants
const std::string TsUtilities::LOGFILE_NAME = "mpeg2ts_log.csv";

TsUtilities::TsUtilities()
    : mPmtPids{}
    , mPmts{}
    , mEsPids{}
    , mAddedPmts{ false }
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
    auto f = [](const mpeg2ts::ByteVector& rawTable, mpeg2ts::PsiTable* table, int aPid,
                void* hdl) { PATCallback(rawTable, table, aPid, hdl); };
    mDemuxer.addPsiPid(mpeg2ts::TS_PACKET_PID_PAT, f, reinterpret_cast<void*>(this));
}

void TsUtilities::registerPmtCallback()
{
    if (!mAddedPmts && !mPmtPids.empty())
    {
        for (auto pid : mPmtPids)
        {
            LOGD << "Adding PSI PID for parsing: " << pid;
            auto f = [](const mpeg2ts::ByteVector& rawTable, mpeg2ts::PsiTable* table, int aPid,
                        void* hdl) { PMTCallback(rawTable, table, aPid, hdl); };
            mDemuxer.addPsiPid(pid, f, reinterpret_cast<void*>(this));
        }
        mAddedPmts = true;
    }
}

void TsUtilities::registerPesCallback()
{
    for (auto pid : mEsPids)
    {
        LOGD << "Adding PES PID for parsing: " << pid;
        auto f = [](const mpeg2ts::ByteVector& a_rawPes, const mpeg2ts::PesPacket& a_pes, int a_pid,
                    void* a_hdl) { PESCallback(a_rawPes, a_pes, a_pid, a_hdl); };
        mDemuxer.addPesPid(pid, f, reinterpret_cast<void*>(this));
    }
}


bool TsUtilities::parseTransportFile(const std::string& a_file)
{
    initParse();
    std::ifstream tsFile(a_file, std::ifstream::binary);

    if (!tsFile)
    {
        return false;
    }

    LOGD << "Parsing tsfile:" << a_file;

    int no = 0;
    while (!tsFile.eof())
    {
        LOGV << "packet no: " << no;
        uint8_t packet[188];
        tsFile.read(reinterpret_cast<char*>(packet), 188); // TODO check sync byte
     
        // Phase lock to sync byte for 1st-packet
        int pos = 0;
        if (no == 0)
        {
            while (packet[pos] != mpeg2ts::TS_PACKET_SYNC_BYTE)
            {
                LOGV << "need to sync...";
                ++pos;
            }
            // rewind and re-read
            tsFile.clear();
            tsFile.seekg(0);
            tsFile.ignore(pos);
            tsFile.read(reinterpret_cast<char*>(packet), 188);
        }


        mDemuxer.demux(&packet[0]);
        ++no;
    }
    tsFile.close();

    return true;
}

bool TsUtilities::parseTransportUdpStream(const IpAddress& /* a_ip*/, const Port& /* a_port*/)
{
    return false;
}


bool TsUtilities::parseTransportStreamData(const uint8_t* a_data, std::size_t a_size)
{
    initParse();

    // If empty data, just return
    if ((a_data == NULL) || (a_data == nullptr))
    {
        LOGE << "No data to parse...";
        return false;
    }

    uint64_t count = 0;
    uint64_t readIndex = 0;


    if ((a_data[0] != mpeg2ts::TS_PACKET_SYNC_BYTE) || (a_size <= 0)) // TODO support maxsize?
    {
        LOGE << "ERROR: 1'st byte not in sync!!!";
        return false;
    }

    while (readIndex < a_size)
    {
        mDemuxer.demux(a_data + readIndex);
        readIndex += mpeg2ts::TS_PACKET_SIZE;
        count++;
    }
    LOGD << "Found " << count << " ts packets.";

    return true;
}


void TsUtilities::PATCallback(const mpeg2ts::ByteVector& /* rawPes*/, mpeg2ts::PsiTable* a_table, int a_pid, void* a_hdl)
{
    auto instance = reinterpret_cast<TsUtilities*>(a_hdl); // TODO try/catch
    LOGV_(FileLog) << "PATCallback pid:" << a_pid;
    mpeg2ts::PatTable* pat;
    try
    {
        pat = dynamic_cast<mpeg2ts::PatTable*>(a_table);
    }
    catch (std::exception& ex)
    {
        LOGE_(FileLog) << "ERROR: dynamic_cast ex: " << ex.what();
        return;
    }

    if (pat == nullptr)
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
    std::size_t numPrograms = pat->programs.size();
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

    // TODO: add writing of a_table
}

mpeg2ts::PatTable TsUtilities::getPatTable() const
{
    return mPrevPat;
}

std::vector<uint16_t> TsUtilities::getPmtPids() const
{
    return mPmtPids;
}

void TsUtilities::PMTCallback(const mpeg2ts::ByteVector& /* rawPes*/, mpeg2ts::PsiTable* a_table, int a_pid, void* a_hdl)
{
    auto instance = reinterpret_cast<TsUtilities*>(a_hdl);

    LOGV_(FileLog) << "PMTCallback... pid:" << a_pid;
    mpeg2ts::PmtTable* pmt;

    try
    {
        pmt = dynamic_cast<mpeg2ts::PmtTable*>(a_table);
    }
    catch (std::exception& ex)
    {
        LOGE_(FileLog) << "ERROR: dynamic_cast ex: " << ex.what();
        return;
    }

    if (pmt == nullptr)
    {
        LOGE_(FileLog) << "ERROR: This should not happen. You have some corrupt stream!!!";
        return;
    }

    // Do nothing if same PMT
    if (instance->mPmts.find(a_pid) != instance->mPmts.end())
    {
        LOGV_(FileLog) << "Got same PMT...";
        return;
    }

    LOGD << "Adding PMT to list with PID: " << a_pid;
    instance->mPmts[a_pid] = *pmt;

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
            //LOGD << "Add PCR PID: " << pmt->PCR_PID << '\n';
            instance->mEsPids.push_back(pmt->PCR_PID);
        }
    }*/

    instance->registerPesCallback();
}

std::map<int, mpeg2ts::PmtTable> TsUtilities::getPmtTables() const
{
    return mPmts;
}

std::vector<uint16_t> TsUtilities::getEsPids() const
{
    return mEsPids;
}

void TsUtilities::PESCallback(const mpeg2ts::ByteVector& a_rawPes, const mpeg2ts::PesPacket& a_pes, int a_pid, void* a_hdl)
{
    auto instance = reinterpret_cast<TsUtilities*>(a_hdl);

    LOGV << "Adding PES to list...";
    instance->mPesPackets[a_pid].push_back(a_pes);

    for (auto& pmtPid : instance->mPmtPids)
    {
        if (instance->mPmts.find(pmtPid) != instance->mPmts.end())
        {
            auto it = std::find_if(instance->mPmts[pmtPid].streams.begin(),
                                   instance->mPmts[pmtPid].streams.end(),
                                   [&](mpeg2ts::StreamTypeHeader& stream) {
                                       return stream.elementary_PID == a_pid;
                                   });
            if (it != instance->mPmts[pmtPid].streams.end())
            {
                try
                {
                    if (it->stream_type == mpeg2ts::STREAMTYPE_VIDEO_MPEG2)
                    {
                        instance->mVideoMediaInfo.codec = VideoCodecType::MPEG2;
                        instance->mVideoMediaInfo.mediaType = MediaType::Video;
                        instance->mVideoMediaInfo.PID = a_pid;
                        std::vector<uint8_t>::const_iterator first = a_rawPes.begin() + a_pes.elementary_data_offset;
                        std::vector<uint8_t>::const_iterator last = a_rawPes.end();
                        std::vector<uint8_t> newVec(first, last);

                        mpeg2::Mpeg2VideoEsParser mpeg2Parser;
                        std::vector<mpeg2::EsInfoMpeg2> ret = mpeg2Parser.parse(newVec);

                        instance->mVideoCallback(newVec, it->stream_type);

                        for (const mpeg2::EsInfoMpeg2& info : ret)
                        {
                            // LOGD << "mpeg2 picture: " << info.picture << " " << info.msg;
                            if (info.type == mpeg2::Mpeg2Type::SliceCode)
                            {
                                instance->mVideoMediaInfo.picType = info.slice.picType;
                                // LOGD << "mpeg2 picture type: " << info.slice.picType << " " <<
                                // info.msg;
                            }
                            else if (info.type == mpeg2::Mpeg2Type::SequenceHeader)
                            {
                                instance->mVideoMediaInfo.width = info.sequence.width;
                                instance->mVideoMediaInfo.height = info.sequence.height;
                                instance->mVideoMediaInfo.frameRate = info.sequence.framerate;
                                instance->mVideoMediaInfo.aspect = info.sequence.aspect;
                            }
                        }
                    } // STREAMTYPE_VIDEO_MPEG2

                    if (it->stream_type == mpeg2ts::STREAMTYPE_VIDEO_H264)
                    {
                        instance->mVideoMediaInfo.codec = VideoCodecType::H264;
                        instance->mVideoMediaInfo.mediaType = MediaType::Video;
                        instance->mVideoMediaInfo.PID = a_pid;
                        std::vector<uint8_t>::const_iterator first = a_rawPes.begin() + a_pes.elementary_data_offset;
                        std::vector<uint8_t>::const_iterator last = a_rawPes.end();
                        std::vector<uint8_t> newVec(first, last);

                        h264::H264EsParser h264Parser;
                        std::vector<h264::EsInfoH264> ret = h264Parser.parse(newVec);

                        instance->mVideoCallback(newVec, it->stream_type);

                        for (const h264::EsInfoH264& info : ret)
                        {
                            // LOGD << "nal: " << h264::H264EsParser::toString(info.nalUnitType) <<
                            // " " << info.msg;
                            if (info.type == h264::H264InfoType::SliceHeader)
                            {
                                // LOGD << info.slice.sliceTypeStr << ", pps id: " <<
                                // info.slice.ppsId;
                                if (info.slice.field)
                                {
                                    // LOGD << "field encoded: " << (info.slice.top ? " top" : "
                                    // bottom");
                                }
                                else
                                {
                                    // LOGD << "frame encoded";
                                }
                            }
                            else if (info.type == h264::H264InfoType::SequenceParameterSet)
                            {
                                instance->mVideoMediaInfo.width = info.sps.width;
                                instance->mVideoMediaInfo.height = info.sps.height;
                                instance->mVideoMediaInfo.lumaBits = info.sps.lumaBits;
                                instance->mVideoMediaInfo.chromaBits = info.sps.chromaBits;
                                instance->mVideoMediaInfo.numRefPics = info.sps.numRefPics;

                                // LOGD << "sps id: " << info.sps.spsId << ", luma bits: " <<
                                // info.sps.lumaBits
                                //      << ", chroma bits: " << info.sps.chromaBits << ", width: "
                                //      << info.sps.width
                                //      << " x " << info.sps.height << ", ref pic: " <<
                                //      info.sps.numRefPics;
                            }
                            else if (info.type == h264::H264InfoType::PictureParameterSet)
                            {
                                // LOGD << "sps id: " << info.pps.spsId << "pps id: " <<
                                // info.pps.ppsId;
                            }
                        }
                    } // STREAMTYPE_VIDEO_H264
                }
                catch (const std::out_of_range&)
                {
                    LOGE << "No parser for stream type " << mpeg2ts::StreamTypeToString[it->stream_type];
                }
            }
        }
    }
}


std::map<int, std::vector<mpeg2ts::PesPacket>> TsUtilities::getPesPackets() const
{
    return mPesPackets;
}

mpeg2ts::PidStatisticsMap TsUtilities::getPidStatistics() const
{
    return mDemuxer.getPidStatistics();
}

void TsUtilities::addVideoCallback(VideoCallBackFnc cb)
{
    mVideoCallback = cb;
}


} // namespace tsutil
