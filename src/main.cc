/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <unistd.h>
#include <list>
#include <cstdlib> // EXIT_SUCCESS

/// Project files
#include "TsDemuxer.h"
#include "TsPacketInfo.h"
#include "TsStandards.h"


uint64_t count = 0;
uint64_t countAdaptPacket = 0;
std::vector<uint16_t> g_PMTPIDS;
std::vector<uint16_t> g_ESPIDS;
TsDemuxer g_tsDemux;
PatTable g_prevPat;
std::list<PmtTable> g_prevPmts;
bool addedPmts = false;

enum class OptionWriteMode
{
    TS,
    PES,
    ES
};

std::map<std::string, std::vector<int>> g_Options;
std::list<OptionWriteMode> g_WriteMode;
std::string g_InputFile;

static const char* optString = "m:w:i:l:h?";

struct option longOpts[] = { { "write", 1, nullptr, 'w' },
                             { "wrmode", 1, nullptr, 'm' },
                             { "pid", 1, nullptr, 'p' },
                             { "level", 1, nullptr, 'l' },
                             { "input", 1, nullptr, 'i' },
                             { "help", 0, nullptr, 'h' },
                             { nullptr, 0, nullptr, 0 } };

bool hasPid(std::string param, uint32_t pid)
{
    return std::count(g_Options[param].begin(), g_Options[param].end(), pid);
}

bool hasPids(std::string param, std::vector<uint16_t> pids)
{
    bool ret;
    for (auto pid : pids)
    {
        ret = std::count(g_Options[param].begin(), g_Options[param].end(), pid);
    }
    return ret;
}

bool hasPmt(const PmtTable& pmt)
{
    for(auto prevPmt : g_prevPmts) {
        if (pmt == prevPmt) {
            return true;
        }
    }
    return false;
}

void display_usage()
{
    std::cout << "Ts-lib simple command-line:" << std::endl;

    std::cout << "USAGE: ./tsparser [-h] [-w PID] [-p PID] [-l log-level] [-i file]" << std::endl;

    std::cout << "Option Arguments:\n"
                 "        -h [ --help ]        Print help messages\n"
                 "        -p [ --pid PID]      Print PSI tables info with PID\n"
                 "        -w [ --write PID]    Writes PES packets with PID to file\n"
                 "        -m [ --wrmode type]  Choose what type of data is written[ts|pes|es]\n"
                 "        -i [ --input FILE]   Use input file for parsing"
              << std::endl;
}

void display_statistics(TsDemuxer demuxer)
{
    for (auto& pidStat : demuxer.getTsStatistics().mPidStatistics)
    {
        if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), pidStat.first) == 0)
        {
            continue;
        }
        std::cout << "Pid: " << pidStat.first << "\n";
        std::cout << " Transport Stream Discontinuity: " << pidStat.second.numberOfTsDiscontinuities
                  << "\n";
        std::cout << " CC error: " << pidStat.second.numberOfCCErrors << "\n";
        std::cout << " Pts differences histogram:\n";
        for (auto& ent : pidStat.second.ptsHistogram)
        {
            std::cout << "  diff: " << ent.first << " quantity " << ent.second << "\n";
        }
        std::cout << " Pts missing: " << pidStat.second.numberOfMissingPts << "\n";

        std::cout << " Dts differences histogram:\n";
        for (auto& ent : pidStat.second.dtsHistogram)
        {
            std::cout << "  diff: " << ent.first << " quantity " << ent.second << "\n";
        }
        std::cout << " Dts missing: " << pidStat.second.numberOfMissingDts << "\n";
        std::cout << " Pcr differences histogram:\n";
        for (auto& ent : pidStat.second.pcrHistogram)
        {
            std::cout << "  diff: " << ent.first << " quantity " << ent.second << "\n";
        }
    }
}



void TsCallback(const uint8_t* packet, TsPacketInfo tsPacketInfo)
{
    auto pid = tsPacketInfo.pid;
    std::cout << "demuxed TS packet \n" << tsPacketInfo;
    if (hasPid("pid", pid))
    {
        std::cout << tsPacketInfo << "\n";
    }

    if (hasPid("write", pid))
    {
        static std::map<uint16_t, std::ofstream> outFiles;
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

        std::cout << "Write TS: " << TS_PACKET_SIZE << " bytes, pid: " << pid << std::endl;
    }
}

void PATCallback(PsiTable* table)
{
    auto pat = dynamic_cast<PatTable*>(table);

    // Do nothing if same PAT
    if (g_prevPat == *pat)
    {
        return;
    }
    g_prevPat = *pat;

    if (hasPid("pid", TS_PACKET_PID_PAT))
    {
        std::cout << "PAT at Ts packet: " << g_tsDemux.getTsStatistics().mTsPacketCounter << "\n";
        std::cout << *pat << std::endl;
    }

    // Check if MPTS or SPTS
    int numPrograms = pat->programs.size();
    if (numPrograms == 0)
    {
        std::cout << "No programs found in PAT, exiting..." << std::endl;
        exit(EXIT_SUCCESS);
    }
    else if (numPrograms == 1) // SPTS
    {
        std::cout << "Found Single Program Transport Stream (SPTS)." << std::endl;
        g_PMTPIDS.push_back(pat->programs[0].program_map_PID);
    }
    else if (numPrograms >= 1) // MPTS
    {
        std::cout << "Found Multiple Program Transport Stream (MPTS)." << std::endl;
        for(auto program : pat->programs) {
            g_PMTPIDS.push_back(program.program_map_PID);
        }
    }


    //TODO: add writing of table
}

void PMTCallback(PsiTable* table)
{
    auto pmt = dynamic_cast<PmtTable*>(table);

    // Do nothing if same PMT
    if (hasPmt(*pmt))
    {
        return;
    }
    g_prevPmts.push_back(*pmt);

    if (hasPids("pid", g_PMTPIDS))
    {
        std::cout << "PMT at Ts packet: " << g_tsDemux.getTsStatistics().mTsPacketCounter << "\n";
        std::cout << *pmt << std::endl;
    }

    for (auto& stream : pmt->streams)
    {
        if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), stream.elementary_PID) ||
            std::count(g_Options["write"].begin(), g_Options["write"].end(), stream.elementary_PID))
        {
            std::cout << "Add ES PID: " << stream.elementary_PID << std::endl;
            g_ESPIDS.push_back(stream.elementary_PID);
        }
    }
    if (pmt->PCR_PID != 0)
    {
        if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), pmt->PCR_PID) ||
            std::count(g_Options["write"].begin(), g_Options["write"].end(), pmt->PCR_PID))
        {
            std::cout << "Add PCR PID: " << pmt->PCR_PID << std::endl;
            g_ESPIDS.push_back(pmt->PCR_PID);
        }        
    }
}

void PESCallback(const PesPacket& pes, uint16_t pid)
{

    if (hasPid("pid", pid))
    {
        std::cout << "PES ENDING at Ts packet " << g_tsDemux.getTsStatistics().mTsPacketCounter
                  << " (" << pid << ")\n";
        std::cout << pes << std::endl;
    }

    if (hasPid("write", pid))
    {
        auto writeOffset = 0;
        auto writeModeString = "";
        if (g_WriteMode.front() == OptionWriteMode::TS)
        {
            return;
        }
        else if (g_WriteMode.front() == OptionWriteMode::PES)
        {
            writeOffset = 0;
            writeModeString = "PES";
        }
        else{
            writeOffset = pes.elementary_data_offset;
            writeModeString = "ES";
        }

        static std::map<uint16_t, std::ofstream> outFiles;
        auto fit = outFiles.find(pid);
        if (fit == outFiles.end())
        {
            outFiles[pid] = std::ofstream(std::to_string(pid) + "." + writeModeString,
                                          std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        }

        std::copy(pes.mPesBuffer.begin() + writeOffset, pes.mPesBuffer.end(), std::ostreambuf_iterator<char>(outFiles[pid]));

        std::cout << "Write " << writeModeString << ": " << pes.mPesBuffer.size() - writeOffset << " bytes, pid: " << pid << std::endl;
    }
}

int main(int argc, char** argv)
{
    std::cout << "Starting parser of file" << std::endl;

    int longIndex;

    int opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
    while (opt != -1)
    {
        switch (opt)
        {
        case 'h': /* fall-through is intentional */
        case '?': {
            display_usage();
            exit(EXIT_SUCCESS);
            break;
        }
        case 'w':
        case 'p':
        case 'l':
            g_Options[longOpts[longIndex].name].push_back(std::atoi(optarg));
            break;
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
            else{
                std::cerr << "Allowed values for write mode are: ts, pes, es";
                display_usage();
                exit(EXIT_FAILURE);
            }
                g_WriteMode.push_back(writeMode);
            }
            break;
        case 'i': {
            std::cout << "Got file input: " << std::string(optarg) << std::endl;
            g_InputFile = std::string(optarg);
            break;
        }
        default:
            /* You won't actually get here. */
            break;
        }

        opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
    }
    if (g_WriteMode.empty())
    {
        g_WriteMode.push_back(OptionWriteMode::PES);
    }
    
    if (g_WriteMode.front() == OptionWriteMode::TS)
    {
        for (auto pid : g_Options["write"])
        {
            g_tsDemux.addTsPid(pid, std::bind(&TsCallback, std::placeholders::_1, std::placeholders::_2), nullptr);
        }
    }
    
    uint64_t count;

    // FILE
    FILE *fptr;
    fptr = fopen(g_InputFile.c_str(), "rb");

    // Find PAT
    g_tsDemux.addPsiPid(TS_PACKET_PID_PAT, std::bind(&PATCallback, std::placeholders::_1), nullptr);

    for (count = 0;; ++count)
    {

        unsigned char packet[TS_PACKET_SIZE];
        // SYNC
        // Check for the sync byte. When found start a new ts-packet parser...
        char b;

        b = fgetc(fptr);
        while (b != TS_PACKET_SYNC_BYTE)
        {
            b = fgetc(fptr);
            int eof = feof(fptr);
            if (eof != 0)
            {
                std::cout << "End Of File..." << std::endl;
                std::cout << "Found " << count << " ts-packets." << std::endl;
                std::cout << "Found Adaptation Field packets:" << countAdaptPacket << " ts-packets."
                          << std::endl;

                std::cout << "Statistics\n";
                display_statistics(g_tsDemux);
                return EXIT_SUCCESS;
            }
        }

        // TS Packet start
        packet[0] = b;

        // Read TS Packet from file
        size_t res = fread(packet + 1, 1, TS_PACKET_SIZE - 1, fptr); // Copy only packet-size - sync byte
        (void)res;

        g_tsDemux.demux(packet);
        if (!addedPmts && (g_PMTPIDS.size() != 0u))
        {
            for (auto pid : g_PMTPIDS)
            {
                std::cout << "Adding PSI PID for parsing: " << pid << std::endl;
                g_tsDemux.addPsiPid(pid, std::bind(&PMTCallback, std::placeholders::_1), nullptr);
            }
            addedPmts = true;
        }

        for (auto pid : g_ESPIDS)
        {
            std::cout << "Adding PES PID for parsing: " << pid << std::endl;
            g_tsDemux.addPesPid(pid, std::bind(&PESCallback, std::placeholders::_1, std::placeholders::_2), nullptr);
        }
        g_ESPIDS.clear();

    } // for loop
}
