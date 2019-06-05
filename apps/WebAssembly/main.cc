/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstdlib> // EXIT_SUCCESS
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <type_traits>
#include <unistd.h>

// 3rd-party
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>
#include <nlohmann/json.hpp>
#include "emscripten.h"

// Project files
#include "Logging.h"
#include "TsParser.h"
#include "Ts_IEC13818-1.h"
#include "h264/H264Parser.h"
#include "mpeg2ts.h"
#include "mpeg2ts_version.h" // Returns current version of mpeg2ts library
#include "mpeg2vid/Mpeg2VideoParser.h"

using namespace mpeg2ts;


uint64_t countAdaptPacket = 0;
std::vector<uint16_t> g_PMTPIDS;
std::vector<uint16_t> g_ESPIDS;
std::unique_ptr<mpeg2ts::TsDemuxer> g_tsDemux;
PatTable g_prevPat;
std::map<int, PmtTable> g_prevPmts;
bool g_Auto = false;
nlohmann::json g_BigJson;
std::stringstream g_BigJsonString;
bool addedPmts{ false };

const char LOGFILE_NAME[]{ "tsparser.csv" };
int LOGFILE_MAXSIZE{ 100 * 1024 };
int LOGFILE_MAXNUMBEROF{ 10 };

const plog::Severity DEFAULT_LOG_LEVEL = plog::error;

enum class OptionWriteMode
{
    TS,
    PES,
    ES
};

std::map<std::string, std::vector<int>> g_Options;
std::list<OptionWriteMode> g_WriteMode;

bool hasPid(std::string param, int pid)
{
    return std::count(g_Options[param].begin(), g_Options[param].end(), pid);
}

bool hasPids(std::string param, std::vector<uint16_t> pids)
{
    bool ret{ false };
    for (auto pid : pids)
    {
        ret += std::count(g_Options[param].begin(), g_Options[param].end(), pid);
    }
    return ret;
}

void display_statistics(mpeg2ts::PidStatisticsMap statistics)
{
    g_BigJson["stream"]["stats"] = nlohmann::json::object();
    for (auto& pidStat : statistics)
    {
        auto& pidStatistic = g_BigJson["stream"]["stats"]["Pid" + std::to_string(pidStat.first)];
        pidStatistic["discontinuity"] = pidStat.second.numberOfTsDiscontinuities;
        pidStatistic["ccError"] = pidStat.second.numberOfCCErrors;
        pidStatistic["PtsHist"] = nlohmann::json::array({});
        pidStatistic["Ptsmissing"] = pidStat.second.numberOfMissingPts;
        for (auto& ent : pidStat.second.ptsHistogram)
        {
            nlohmann::json jsonHist;
            jsonHist["d"] = ent.first;
            jsonHist["n"] = ent.second;
            pidStatistic["PtsHist"].push_back(jsonHist);
        }
        
        pidStatistic["DtsHist"] = nlohmann::json::array({});
        pidStatistic["Dtsmissing"] = pidStat.second.numberOfMissingDts;
        for (auto& ent : pidStat.second.dtsHistogram)
        {
            nlohmann::json jsonHist;
            jsonHist["d"] = ent.first;
            jsonHist["n"] = ent.second;
            pidStatistic["DtsHist"].push_back(jsonHist);
        }
        /*for (auto& ent : pidStat.second.pcrHistogram)
        {
            LOGD << "  diff: " << ent.first << " quantity " << ent.second << '\n';
        }*/
    }
}


void TsCallback(const uint8_t* packet, TsPacketInfo tsPacketInfo)
{
    auto pid{ tsPacketInfo.pid };
    LOGD << "demuxed TS packet \n" << tsPacketInfo;
    if (hasPid("pid", pid))
    {
        LOGN << tsPacketInfo;
    }

    if (hasPid("write", pid))
    {
        static std::map<int, std::ofstream> outFiles;
        auto fit = outFiles.find(pid);
        if (fit == outFiles.end())
        {
            outFiles[pid] = std::ofstream(std::to_string(pid) + ".ts",
                                          std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        }
        if (g_WriteMode.front() != OptionWriteMode::TS)
        {
            return;
        }
        std::copy(packet, packet + TS_PACKET_SIZE, std::ostreambuf_iterator<char>(outFiles[pid]));

        LOGD << "Write TS: " << TS_PACKET_SIZE << " bytes, pid: " << pid << '\n';
    }
}

void PATCallback(const ByteVector& /* rawPes*/, PsiTable* table, int pid)
{
    LOGV << "PATCallback pid:" << pid;
    
    if (g_Auto)
    {
        nlohmann::json jsonPat;
        jsonPat["ofs"] = g_tsDemux->getOrigin(pid);
        g_BigJson["stream"]["Pid0"].push_back(jsonPat);
    }
    
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

    if (pat == nullptr)
    {
        LOGE_(FileLog) << "ERROR: This should not happen. You have some corrupt stream!!!";
        return;
    }

    // Do nothing if same PAT
    if (g_prevPat == *pat)
    {
        LOGV << "Got same PAT...";
        return;
    }
    g_prevPat = *pat;

    if (hasPid("pid", TS_PACKET_PID_PAT))
    {
        LOGN << "PAT at Ts packet: " << g_tsDemux->getTsCounters().mTsPacketCounter << '\n';
        LOGN << *pat << '\n';
    }
    
    if (g_Auto)
    {
        g_BigJson["stream"]["Pid0"].back()["pmtPids"] = nlohmann::json::array();
    }

    // new PAT clear everything
    //TODO: clear pids from g_tsDemux-> currently no API
    g_PMTPIDS.clear();
    addedPmts = false;

    // Check if MPTS or SPTS
    auto numPrograms = pat->programs.size();
    if (numPrograms == 0)
    {
        LOGD << "No programs found in PAT, exiting...";
        exit(EXIT_SUCCESS);
    }
    else if (numPrograms == 1) // SPTS
    {
        LOGD << "Found Single Program Transport Stream (SPTS).";
        g_PMTPIDS.push_back(pat->programs[0].program_map_PID);
        if (g_Auto)
        {
            g_BigJson["stream"]["Pid0"].back()["pmtPids"].push_back(pat->programs[0].program_map_PID);
        }
    }
    else if (numPrograms >= 1) // MPTS
    {
        LOGD << "Found Multiple Program Transport Stream (MPTS).";
        for (auto program : pat->programs)
        {
            if (program.type == ProgramType::PMT)
            {
                g_PMTPIDS.push_back(program.program_map_PID);
                if (g_Auto)
                {
                    g_BigJson["stream"]["Pid0"].back()["pmtPids"].push_back(program.program_map_PID);
                }
            }
        }
    }


    // TODO: add writing of table
}

void PMTCallback(const ByteVector& /* rawPes*/, PsiTable* table, int pid)
{
    LOGV << "PMTCallback... pid:" << pid;
    PmtTable* pmt;
    
    if (g_Auto)
    {
        try
        {
            auto& pmtArray = g_BigJson["stream"].at("Pid" + std::to_string(pid));
            nlohmann::json jsonPmt;
            jsonPmt["ofs"] = g_tsDemux->getOrigin(pid);
            pmtArray.push_back(jsonPmt);
        }
        catch(...)
        {
            nlohmann::json jsonPmt;
            jsonPmt["ofs"] = g_tsDemux->getOrigin(pid);
            g_BigJson["stream"]["Pid" + std::to_string(pid)] = nlohmann::json::array({jsonPmt});
        }
    }

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
    if (g_prevPmts.find(pid) != g_prevPmts.end()) //TODO: g_prevPmts.find(pid) != *pmt, right ???
    {
        LOGV << "Got same PMT...";
        return;
    }

    g_prevPmts[pid] = *pmt;

    if (hasPids("pid", g_PMTPIDS))
    {
        LOGN << "PMT at Ts packet: " << g_tsDemux->getTsCounters().mTsPacketCounter;
        LOGN << *pmt;
    }
    
    if (g_Auto)
    {
        g_BigJson["stream"]["Pid" + std::to_string(pid)].back()["streams"] = nlohmann::json::array();
    }

    for (auto& stream : pmt->streams)
    {
        if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), stream.elementary_PID) ||
            std::count(g_Options["write"].begin(), g_Options["write"].end(), stream.elementary_PID) || g_Auto)
        {
            LOGD << "Add ES PID: " << stream.elementary_PID << '\n';
            g_ESPIDS.push_back(stream.elementary_PID);
        }
        
        if (g_Auto)
        {
            nlohmann::json pmtEntry;
            pmtEntry["pid"] = stream.elementary_PID;
            pmtEntry["type"] = StreamTypeToString[stream.stream_type];
            g_BigJson["stream"]["Pid" + std::to_string(pid)].back()["streams"].push_back(pmtEntry);
        }
    }
    if (pmt->PCR_PID != 0)
    {
        if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), pmt->PCR_PID) ||
            std::count(g_Options["write"].begin(), g_Options["write"].end(), pmt->PCR_PID) || g_Auto)
        {
            LOGD << "Add PCR PID: " << pmt->PCR_PID << '\n';
            g_ESPIDS.push_back(pmt->PCR_PID);
        }
    }
}

void PESCallback(const ByteVector& rawPes, const PesPacket& pes, int pid)
{
    if (hasPid("pid", pid))
    {
        LOGN << "PES ENDING at Ts packet " << g_tsDemux->getTsCounters().mTsPacketCounter << " (" << pid << ")\n";
        LOGN << pes << '\n';
    }
    
    if (g_Auto)
    {
        try
        {
            auto& pesArray = g_BigJson["stream"].at("Pid" + std::to_string(pid));
            nlohmann::json jsonPes;
            jsonPes["ofs"] = g_tsDemux->getOrigin(pid);
            if (pes.dts >= 0)
            {
                jsonPes["dts"] = pes.dts;
            }
            if (pes.pts >= 0)
            {
                jsonPes["pts"] = pes.pts;
            }
            pesArray.push_back(jsonPes);
        }
        catch(...)
        {
            nlohmann::json jsonPes;
            jsonPes["ofs"] = g_tsDemux->getOrigin(pid);
            if (pes.dts >= 0)
            {
                jsonPes["dts"] = pes.dts;
            }
            if (pes.pts >= 0)
            {
                jsonPes["pts"] = pes.pts;
            }
            g_BigJson["stream"]["Pid" + std::to_string(pid)] = nlohmann::json::array({jsonPes});
        }
    }    
    
    // @TODO add "if parse pid" option to cmd line
    {
        for (auto& pmtPid : g_PMTPIDS)
        {
            if (g_prevPmts.find(pmtPid) != g_prevPmts.end())
            {
                auto it =
                std::find_if(g_prevPmts[pmtPid].streams.begin(), g_prevPmts[pmtPid].streams.end(),
                             [&](StreamTypeHeader& stream) { return stream.elementary_PID == pid; });
                if (it != g_prevPmts[pmtPid].streams.end())
                {
                    try
                    {
                        if (it->stream_type == STREAMTYPE_VIDEO_MPEG2)
                        {
                            std::vector<uint8_t>::const_iterator first = rawPes.begin() + pes.elementary_data_offset;
                            std::vector<uint8_t>::const_iterator last = rawPes.end();
                            std::vector<uint8_t> newVec(first, last);

                            mpeg2::Mpeg2VideoEsParser mpeg2Parser;
                            std::vector<mpeg2::EsInfoMpeg2> infos = mpeg2Parser.parse(newVec);

                            for (auto info : infos)
                            {
                                LOGD << "----------------------------------------------";
                                LOGD << "mpeg2 bytestream type: "
                                     << mpeg2::Mpeg2VideoEsParser::toString(info.type);
                                LOGD << "mpeg2 picture: " << info.picture << " " << info.msg;
                                if (info.type == mpeg2::Mpeg2Type::SliceCode)
                                {
                                    LOGD << "mpeg2 picture type: " << info.slice.picType << " "
                                         << info.msg;
                                    if (info.slice.picType == 1) // I pic
                                    {
                                        g_BigJson["stream"]["Pid" + std::to_string(pid)].back()["extra"] += "I frame";
                                    }
                                }
                                else if (info.type == mpeg2::Mpeg2Type::SequenceHeader)
                                {
                                    LOGD << "size: " << info.sequence.width << " x " << info.sequence.height
                                         << ", aspect: " << info.sequence.aspect
                                         << ", frame rate: " << info.sequence.framerate;
                                    std::stringstream tmp;
                                    tmp << "size: " << info.sequence.width << " x " << info.sequence.height;
                                    g_BigJson["stream"]["Pid" + std::to_string(pid)].back()["extra"] += tmp.str();
                                }
                            }
                        } // STREAMTYPE_VIDEO_MPEG2

                        if (it->stream_type == STREAMTYPE_VIDEO_H264)
                        {
                            std::vector<uint8_t>::const_iterator first = rawPes.begin() + pes.elementary_data_offset;
                            std::vector<uint8_t>::const_iterator last = rawPes.end();
                            std::vector<uint8_t> newVec(first, last);

                            h264::H264EsParser h264Parser;
                            std::vector<h264::EsInfoH264> infos = h264Parser.parse(newVec);
                            g_BigJson["stream"]["Pid" + std::to_string(pid)].back()["nals"] = nlohmann::json::array();
                            for (auto info : infos)
                            {
                                nlohmann::json jsonh264Nal;
                                jsonh264Nal["type"] = info.nalUnitType;
                                g_BigJson["stream"]["Pid" + std::to_string(pid)].back()["nals"].push_back(jsonh264Nal);
                                LOGD << "----------------------------------------------";
                                LOGD << "h264 nal type: " << h264::H264EsParser::toString(info.type);
                                LOGD << "nal: " << h264::H264EsParser::toString(info.nalUnitType)
                                     << " " << info.msg;
                                if (info.type == h264::H264InfoType::SliceHeader)
                                {
                                    LOGD << info.slice.sliceTypeStr << ", pps id: " << info.pps.ppsId;
                                    if (info.slice.field)
                                    {
                                        LOGD << "field encoded: " << (info.slice.top ? " top" : " bottom");
                                    }
                                    else
                                    {
                                        LOGD << "frame encoded";
                                    }
                                }
                                else if (info.type == h264::H264InfoType::SequenceParameterSet)
                                {
                                    LOGD << "sps id: " << info.pps.spsId
                                         << ", luma bits: " << info.sps.lumaBits
                                         << ", chroma bits: " << info.sps.chromaBits
                                         << ", size: " << info.sps.width << " x "
                                         << info.sps.height << ", ref pic: " << info.sps.numRefPics;
                                    std::stringstream tmp;
                                    tmp << ", luma bits: " << info.sps.lumaBits
                                        << ", chroma bits: " << info.sps.chromaBits
                                        << ", size: " << info.sps.width << " x "
                                        << info.sps.height << " " << info.msg;
                                    g_BigJson["stream"]["Pid" + std::to_string(pid)].back()["extra"] = tmp.str();
                                }
                                else if (info.type == h264::H264InfoType::PictureParameterSet)
                                {
                                    LOGD << "sps id: " << info.pps.spsId << "pps id: " << info.pps.ppsId;
                                }
                            }
                        } // STREAMTYPE_VIDEO_H264
                    }
                    catch (const std::out_of_range&)
                    {
                        LOGD << "No parser for stream type " << StreamTypeToString[it->stream_type];
                    }
                }
            }
        }
    }

    if (hasPid("write", pid))
    {
        auto writeOffset{ 0 };
        auto writeModeString{ "" };
        if (g_WriteMode.front() == OptionWriteMode::TS)
        {
            return;
        }
        else if (g_WriteMode.front() == OptionWriteMode::PES)
        {
            writeOffset = 0;
            writeModeString = "PES";
        }
        else
        {
            writeOffset = pes.elementary_data_offset;
            writeModeString = "ES";
        }

        static std::map<int, std::ofstream> outFiles;
        auto fit = outFiles.find(pid);
        if (fit == outFiles.end())
        {
            outFiles[pid] = std::ofstream(std::to_string(pid) + "." + writeModeString,
                                          std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        }

        std::copy(rawPes.begin() + writeOffset, rawPes.end(), std::ostreambuf_iterator<char>(outFiles[pid]));

        LOGD << "Write " << writeModeString << ": " << rawPes.size() - writeOffset
             << " bytes, pid: " << pid << '\n';
    }
}

extern void printTsPacket(const uint8_t* packet)
{
    for (int i = 0; i < 188; ++i)
    {
        printf("0x%02x\n", packet[i]);
    }
    printf("\n");
}

extern "C"
{
EMSCRIPTEN_KEEPALIVE uint32_t webAsmEntryPoint(uint8_t* tsData,
                                               uint32_t tsDataLen,
                                               uint8_t* jsonString)
{
    static plog::RollingFileAppender<plog::CsvFormatter> fileAppender(LOGFILE_NAME, LOGFILE_MAXSIZE, LOGFILE_MAXNUMBEROF); // Create the 1st appender.
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender; // Create the 2nd appender.
    plog::init(DEFAULT_LOG_LEVEL, &fileAppender).addAppender(&consoleAppender); // Initialize the
                                                                                // logger with the
                                                                                // both appenders.
    plog::init<FileLog>(DEFAULT_LOG_LEVEL, &fileAppender); // Initialize the 2nd logger instance.

    LOGD << "Starting parser of file";
    countAdaptPacket = 0;
    g_PMTPIDS.clear();
    g_ESPIDS.clear();
    g_tsDemux = std::unique_ptr<mpeg2ts::TsDemuxer>(new mpeg2ts::TsDemuxer());
    PatTable g_prevPat;
    g_prevPmts.clear();
    g_Auto = true;
    g_BigJson.clear();
    g_BigJsonString.clear();
    addedPmts = false;
    g_WriteMode.clear();

    g_WriteMode.push_back(OptionWriteMode::PES);

    auto f1 = [](const mpeg2ts::ByteVector& rawTable, mpeg2ts::PsiTable* table, int aPid, void* hdl) {
        (void)hdl;
        PATCallback(rawTable, table, aPid);
    };
    g_tsDemux->addPsiPid(TS_PACKET_PID_PAT, f1, nullptr); // Find PAT

    g_BigJson["stream"] = nlohmann::json::object();
    g_BigJson["stream"]["name"] = "wasm";
    g_BigJson["stream"]["Pid0"] = nlohmann::json::array();
    for (size_t tsDataInx = 0; tsDataInx + 188 < tsDataLen; tsDataInx += 188)
    {
        if (tsData[tsDataInx] != TS_PACKET_SYNC_BYTE || 
            tsData[tsDataInx + 188] != TS_PACKET_SYNC_BYTE)
        {
            ++tsDataInx;
            continue;
        }
        auto dataOrigin = tsDataInx;

        TsPacketInfo info;
        TsParser parser;
        try
        {
            parser.parseTsPacketInfo(&tsData[tsDataInx], info);
            g_tsDemux->setOrigin(dataOrigin);
            g_tsDemux->demux(&tsData[tsDataInx]);
        }
        catch (GetBitsException& e)
        {
            LOGE_(FileLog) << "Got exception: " << e.what();
            LOGE_(FileLog) << "Got header: " << info.hdr;
            LOGE_(FileLog) << "Got packet: " << info;
            LOGE << "Ignoring exception";
        }
        if (!addedPmts && (g_PMTPIDS.size() != 0u))
        {
            for (auto pid : g_PMTPIDS)
            {
                LOGD << "Adding PSI PID for parsing: " << pid;
                auto f2 = [](const mpeg2ts::ByteVector& rawTable, mpeg2ts::PsiTable* table, int aPid, void* hdl) {
                    (void)hdl;
                    PMTCallback(rawTable, table, aPid);
                };
                g_tsDemux->addPsiPid(pid, f2, nullptr);
            }
            addedPmts = true;
        }

        for (auto pid : g_ESPIDS)
        {
            LOGD << "Adding PES PID for parsing: " << pid;
            auto f3 = [](const ByteVector& rawPes, const PesPacket& pes, int aPid, void* hdl) {
                (void)hdl;
                PESCallback(rawPes, pes, aPid);
            };
            g_tsDemux->addPesPid(pid, f3, nullptr);
        }
        g_ESPIDS.clear();

    } // for loop
    
    display_statistics(g_tsDemux->getPidStatistics());

    auto ret = g_BigJson.dump();
    for (size_t i = 0u; i < ret.size(); ++i)
    {
        jsonString[i] = ret[i];
    }
    return ret.size();

}
}

int main(int argc, char** argv)
{
    return 1;
}
