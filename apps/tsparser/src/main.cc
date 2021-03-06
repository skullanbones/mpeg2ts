/*****************************************************************
*
*    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
*
*    mpeg2ts - mpeg2ts tsparser
*
*    This file is part of mpeg2ts (Mpeg2 Transport Stream Library).
*
*    Unless you have obtained mpeg2ts under a different license,
*    this version of mpeg2ts is mpeg2ts|GPL.
*    Mpeg2ts|GPL is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License as
*    published by the Free Software Foundation; either version 2,
*    or (at your option) any later version.
*
*    Mpeg2ts|GPL is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with mpeg2ts|GPL; see the file COPYING.  If not, write to the
*    Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
*    02111-1307, USA.
*
******************************************************************/
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

// Project files
#include "Ts_IEC13818-1.h"
#include "mpeg2ts.h"
#include "mpeg2ts_version.h" // Returns current version of mpeg2ts library

// Private project files
#include "Mpeg2VideoParser.h"
#include "H264Parser.h"
#include "TsParser.h"


enum // Define log instances. Default is 0 and is omitted from this enum.
{
    FileLog = 1
};

using namespace mpeg2ts;


uint64_t countAdaptPacket = 0;
std::vector<uint16_t> g_PMTPIDS;
std::vector<uint16_t> g_ESPIDS;
mpeg2ts::TsDemuxer g_tsDemux;
PatTable g_prevPat;
std::map<int, PmtTable> g_prevPmts;
bool addedPmts{ false };

const char LOGFILE_NAME[]{ "tsparser.csv" };
int LOGFILE_MAXSIZE{ 100 * 1024 };
int LOGFILE_MAXNUMBEROF{ 10 };

const plog::Severity DEFAULT_LOG_LEVEL = plog::debug;

enum class OptionWriteMode
{
    TS,
    PES,
    ES
};

std::map<std::string, std::vector<int>> g_Options;
std::list<OptionWriteMode> g_WriteMode;
std::string g_InputFile;

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

void display_usage()
{
    printf("Mpeg2ts lib simple command-line:\n");

    printf(
    "USAGE: ./tsparser [-h] [-v] [-p PID] [-w PID] [-m ts|pes|es] [-l log-level] [-i file]\n");

    printf("Option Arguments:\n"
           "        -h [ --help ]        Print help messages\n"
           "        -v [ --version ]     Print library version\n"
           "        -p [ --pid PID]      Print PSI tables info with PID\n"
           "        -w [ --write PID]    Writes PES packets with PID to file\n"
           "        -m [ --wrmode type]  Choose what type of data is written[ts|pes|es]\n"
           "        -l [ --log-level NONE|FATAL|ERROR|WARNING|INFO|DEBUG|VERBOSE] Choose "
           "what logs are filtered, both file and stdout, default: %s\n"
           "        -i [ --input FILE]   Use input file for parsing\n",
           plog::severityToString(DEFAULT_LOG_LEVEL));
}

void display_statistics(mpeg2ts::PidStatisticsMap statistics)
{
    for (auto& pidStat : statistics)
    {
        if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), pidStat.first) == 0)
        {
            continue;
        }
        LOGD << "Pid: " << pidStat.first << '\n';
        LOGD << " Transport Stream Discontinuity: " << pidStat.second.numberOfTsDiscontinuities << '\n';
        LOGD << " CC error: " << pidStat.second.numberOfCCErrors << '\n';
        LOGD << " Pts differences histogram:\n";
        for (auto& ent : pidStat.second.ptsHistogram)
        {
            LOGD << "  diff: " << ent.first << " quantity " << ent.second << '\n';
        }
        LOGD << " Pts missing: " << pidStat.second.numberOfMissingPts << '\n';

        LOGD << " Dts differences histogram:\n";
        for (auto& ent : pidStat.second.dtsHistogram)
        {
            LOGD << "  diff: " << ent.first << " quantity " << ent.second << '\n';
        }
        LOGD << " Dts missing: " << pidStat.second.numberOfMissingDts << '\n';
        LOGD << " Pcr differences histogram:\n";
        for (auto& ent : pidStat.second.pcrHistogram)
        {
            LOGD << "  diff: " << ent.first << " quantity " << ent.second << '\n';
        }
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
        LOGN << "PAT at Ts packet: " << g_tsDemux.getTsCounters().mTsPacketCounter << '\n';
        LOGN << *pat << '\n';
    }

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
    }
    else if (numPrograms >= 1) // MPTS
    {
        LOGD << "Found Multiple Program Transport Stream (MPTS).";
        for (auto program : pat->programs)
        {
            if (program.type == ProgramType::PMT)
            {
                g_PMTPIDS.push_back(program.program_map_PID);
            }
        }
    }


    // TODO: add writing of table
}

void PMTCallback(const ByteVector& /* rawPes*/, PsiTable* table, int pid)
{
    LOGV << "PMTCallback... pid:" << pid;
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
    if (g_prevPmts.find(pid) != g_prevPmts.end())
    {
        LOGV << "Got same PMT...";
        return;
    }

    g_prevPmts[pid] = *pmt;

    if (hasPids("pid", g_PMTPIDS))
    {
        LOGN << "PMT at Ts packet: " << g_tsDemux.getTsCounters().mTsPacketCounter;
        LOGN << *pmt;
    }

    for (auto& stream : pmt->streams)
    {
        if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), stream.elementary_PID) ||
            std::count(g_Options["write"].begin(), g_Options["write"].end(), stream.elementary_PID))
        {
            LOGD << "Add ES PID: " << stream.elementary_PID << '\n';
            g_ESPIDS.push_back(stream.elementary_PID);
        }
    }
    if (pmt->PCR_PID != 0)
    {
        if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), pmt->PCR_PID) ||
            std::count(g_Options["write"].begin(), g_Options["write"].end(), pmt->PCR_PID))
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
        LOGN << "PES ENDING at Ts packet " << g_tsDemux.getTsCounters().mTsPacketCounter << " (" << pid << ")\n";
        LOGN << pes << '\n';
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
                                }
                                else if (info.type == mpeg2::Mpeg2Type::SequenceHeader)
                                {
                                    LOGD << "size: " << info.sequence.width << " x " << info.sequence.height
                                         << ", aspect: " << info.sequence.aspect
                                         << ", frame rate: " << info.sequence.framerate;
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

                            for (auto info : infos)
                            {
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

static const char* optString = "m:w:i:l:p:h?v";

struct option longOpts[] = { { "write", 1, nullptr, 'w' },   { "wrmode", 1, nullptr, 'm' },
                             { "input", 1, nullptr, 'i' },   { "log-level", 1, nullptr, 'l' },
                             { "pid", 1, nullptr, 'p' },     { "help", 0, nullptr, 'h' },
                             { "version", 0, nullptr, 'v' }, { nullptr, 0, nullptr, 0 } };

int main(int argc, char** argv)
{
    // Initialize the logger
    /// Short macros list
    /// LOGV << "verbose";
    /// LOGD << "debug";
    /// LOGI << "info";
    /// LOGW << "warning";
    /// LOGE << "error";
    /// LOGF << "fatal";
    /// LOGN << "none";

    static plog::RollingFileAppender<plog::CsvFormatter> fileAppender(LOGFILE_NAME, LOGFILE_MAXSIZE, LOGFILE_MAXNUMBEROF); // Create the 1st appender.
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender; // Create the 2nd appender.
    plog::init(DEFAULT_LOG_LEVEL, &fileAppender).addAppender(&consoleAppender); // Initialize the
                                                                                // logger with the
                                                                                // both appenders.
    plog::init<FileLog>(DEFAULT_LOG_LEVEL, &fileAppender); // Initialize the 2nd logger instance.

    LOGD << "Starting parser of file";

    for (;;)
    {
        int opt;
        int optInd = -1;
        opt = getopt_long(argc, argv, optString, longOpts, &optInd);
        if (optInd == -1)
        {
            for (optInd = 0; longOpts[optInd].name; ++optInd)
            {
                if (longOpts[optInd].val == opt)
                {
                    LOGD << "optInd: " << optInd;
                    break;
                }
            }
            if (longOpts[optInd].name == NULL)
            {
                // the short option was not found; do something
                LOGE << "the short option was not found; do something"; // TODO
            }
        }

        if (opt < 0)
            break;
        switch (opt)
        {
        case 'h': /* fall-through is intentional */
        case '?':
        {
            display_usage();
            exit(EXIT_SUCCESS);
        }
        case 'v':
        {
            printf("version: %s\n", getMpeg2tsVersion().c_str());
            exit(EXIT_SUCCESS);
        }
        case 'w':
        case 'p':
            LOGD << "Got pid listener pid:" << std::atoi(optarg);
            g_Options[longOpts[optInd].name].push_back(std::atoi(optarg));
            break;
        case 'l':
        {
            LOGD << "Use Default log-level: " << plog::severityToString(DEFAULT_LOG_LEVEL);
            std::string logLevel = std::string(optarg);
            LOGD << "Got input log-level setting: " << logLevel;
            for (auto& c : logLevel)
            {
                c = static_cast<char>(toupper(c));
            }
            plog::Severity severity = plog::severityFromString(logLevel.c_str());
            plog::get()->setMaxSeverity(severity);
            LOGD << "Use log-level: " << plog::severityToString(severity) << ", (" << severity << ")";
            break;
        }
        case 'm':
        {
            OptionWriteMode writeMode = OptionWriteMode::PES;
            if (std::string(optarg) == "ts")
            {
                writeMode = OptionWriteMode::TS;
            }
            else if (std::string(optarg) == "pes")
            {
                writeMode = OptionWriteMode::PES;
            }
            else if (std::string(optarg) == "es")
            {
                writeMode = OptionWriteMode::ES;
            }
            else
            {
                std::cerr << "Allowed values for write mode are: ts, pes, es";
                display_usage();
                exit(EXIT_FAILURE);
            }
            g_WriteMode.push_back(writeMode);
            break;
        }
        case 'i':
        {
            LOGD << "Got file input: " << std::string(optarg);
            g_InputFile = std::string(optarg);
            break;
        }
        default:
            /* You won't actually get here. */
            break;
        }
    } // for

    if (g_WriteMode.empty())
    {
        g_WriteMode.push_back(OptionWriteMode::PES);
    }

    if (g_WriteMode.front() == OptionWriteMode::TS)
    {
        for (auto pid : g_Options["write"])
        {
            auto f = [](const uint8_t* packet, TsPacketInfo tsPacketInfo, void* hdl) {
                (void)hdl;
                TsCallback(packet, tsPacketInfo);
            };
            g_tsDemux.addTsPid(pid, f, nullptr);
        }
    }

    uint64_t count;

    // FILE
    FILE* fptr = NULL;
    fptr = fopen(g_InputFile.c_str(), "rb");

    if (fptr == NULL)
    {
        LOGE << "ERROR: Invalid file! Exiting...";
        exit(EXIT_FAILURE);
    }

    auto f1 = [](const mpeg2ts::ByteVector& rawTable, mpeg2ts::PsiTable* table, int aPid, void* hdl) {
        (void)hdl;
        PATCallback(rawTable, table, aPid);
    };
    g_tsDemux.addPsiPid(TS_PACKET_PID_PAT, f1, nullptr); // Find PAT

    for (count = 0;; ++count)
    {
        unsigned char packet[TS_PACKET_SIZE + 1];
        // SYNC
        // Check for the sync byte. When found start a new ts-packet parser...
        char b;

        b = static_cast<char>(fgetc(fptr));
        while (b != TS_PACKET_SYNC_BYTE)
        {
            // printf("ERROR: Sync error!!!\n");
            b = static_cast<char>(fgetc(fptr));
            int eof = feof(fptr);
            if (eof != 0)
            {
                LOGD << "End Of File..." << '\n';
                LOGD << "Found " << count << " ts-packets." << '\n';
                LOGD << "Found Adaptation Field packets:" << countAdaptPacket << " ts-packets." << '\n';

                LOGD << "Statistics\n";
                display_statistics(g_tsDemux.getPidStatistics());
                fclose(fptr);
                return EXIT_SUCCESS;
            }
        }

        // TS Packet start
        packet[0] = b;

        // Read TS Packet from file
        std::size_t res =
        fread(packet + 1, 1, TS_PACKET_SIZE, fptr); // Copy only packet size + next sync byte
        if (res != TS_PACKET_SIZE)
        {
            LOGE_(FileLog) << "ERROR: Could not read a complete TS-Packet, read: "
                           << res; // May be last packet end of file.
        }
        // printTsPacket(packet);
        // TODO fix this. We are almost always in here where we dont have 2 consecutive synced
        // packets...
        if (packet[TS_PACKET_SIZE] != TS_PACKET_SYNC_BYTE)
        {
            LOGE << "ERROR: Ts-packet Sync error. Next packet sync: ";
            // (int)packet[TS_PACKET_SIZE] << '\n';  fseek(fptr, -TS_PACKET_SIZE, SEEK_CUR);
            // continue; // Skip this packet since it's not synced.
        }
        fseek(fptr, -1, SEEK_CUR); // reset file pointer and skip sync after packet

        TsPacketInfo info;
        TsParser parser;
        try
        {
            parser.parseTsPacketInfo(packet, info);
            g_tsDemux.demux(packet);
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
                g_tsDemux.addPsiPid(pid, f2, nullptr);
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
            g_tsDemux.addPesPid(pid, f3, nullptr);
        }
        g_ESPIDS.clear();

    } // for loop
}
