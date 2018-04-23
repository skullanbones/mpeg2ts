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

/// Project files
#include "TsDemuxer.h"
#include "TsPacketInfo.h"
#include "TsParser.h"
#include "TsStandards.h"


uint64_t count = 0;
uint64_t countAdaptPacket = 0;
uint32_t g_SPPID = 0; // Single Program PID
std::vector<uint16_t> g_ESPIDS;

enum OptionWriteLevel
{
    TS = 1,
    PES = 2,
    ES = 3
};

std::map<std::string, std::vector<int>> g_Options;

static const char* optString = "wil:h?";

struct option longOpts[] = { { "write", 1, nullptr, 'w' },
                             { "info", 1, nullptr, 'i' },
                             { "level", 1, nullptr, 'l' },
                             { "help", 0, nullptr, 'h' },
                             { nullptr, 0, nullptr, 0 } };

bool hasPid(std::string param, uint32_t pid)
{
    return std::count(g_Options[param].begin(), g_Options[param].end(), pid);
}

void display_usage()
{
    std::cout << "Ts-lib simple command-line:" << std::endl;

    std::cout << "USAGE: ./tsparser [-h] [-w PID] [-i PID] [-l log-level]" << std::endl;

    std::cout << "Option Arguments:\n"
                 "        -h [ --help ]        Print help messages\n"
                 "        -i [ --info PID]        print PSI tables info with PID\n"
                 "        -w [ --write PID]       writes PES packets with PID to file"
              << std::endl;
}

TsDemuxer tsDemux;

void TsCallback(unsigned char packet, TsPacketInfo tsPacketInfo)
{
    (void)packet;
    std::cout << "demuxed TS packet \n" << tsPacketInfo;
}

void PATCallback(PsiTable* table)
{
    auto pat = dynamic_cast<PatTable*>(table);
    if (hasPid("info", 0))
    {
        std::cout << "PAT at Ts packent: " << tsDemux.getTsStatistics().mTsPacketCounter << "\n";
        std::cout << *pat << std::endl;
    }

    g_SPPID = pat->programs[0].program_map_PID; // Assume SPTS
}

void PMTCallback(PsiTable* table)
{
    auto pmt = dynamic_cast<PmtTable*>(table);
    if (hasPid("info", g_SPPID))
    {
        std::cout << "PMT at Ts packent: " << tsDemux.getTsStatistics().mTsPacketCounter << "\n";
        std::cout << *pmt << std::endl;
    }

    for (auto& stream : pmt->streams)
    {
        if (std::count(g_Options["info"].begin(), g_Options["info"].end(), stream.elementary_PID) ||
            std::count(g_Options["write"].begin(), g_Options["write"].end(), stream.elementary_PID))
        {
            g_ESPIDS.push_back(stream.elementary_PID);
        }
    }
}

void PESCallback(const PesPacket& pes, uint16_t pid)
{

    if (hasPid("info", pid))
    {
        std::cout << "PES ENDING at Ts packet " << tsDemux.getTsStatistics().mTsPacketCounter
                  << " (" << pid << ")\n";
        std::cout << pes << std::endl;
    }

    if (hasPid("write", pid))
    {
        static std::map<uint16_t, std::ofstream> outFiles;
        auto fit = outFiles.find(pid);
        if (fit == outFiles.end())
        {
            outFiles[pid] = std::ofstream(std::to_string(pid) + ".pes",
                                          std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        }

        std::copy(pes.mPesBuffer.begin(), pes.mPesBuffer.end(), std::ostreambuf_iterator<char>(outFiles[pid]));

        std::cout << "Write " << pes.mPesBuffer.size() << " B of PesPacket on pid: " << pid << std::endl;
    }
}

int main(int argc, char** argv)
{
    std::cout << "Staring parser of stdout" << std::endl;

    int longIndex;
    int opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
    while (opt != -1)
    {

        switch (opt)
        {
        case 'h': /* fall-through is intentional */
        case '?':
            display_usage();
            break;
        case 'w':
        case 'i':
        case 'l':
            g_Options[longOpts[longIndex].name].push_back(std::atoi(optarg));
            break;

        default:
            /* You won't actually get here. */
            break;
        }

        opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
    }

    uint64_t count;

    // Specify input stream
    setvbuf(stdout, NULL, _IOLBF, 0);

    tsDemux.addPsiPid(TS_PACKET_PID_PAT, std::bind(&PATCallback, std::placeholders::_1));

    std::cout << std::boolalpha;
    std::cout << std::is_pod<TsHeader>::value << '\n';
    std::cout << std::is_pod<TsAdaptationFieldHeader>::value << '\n';

    /*
    TODO: move it to gtest
    tsParser.parseTsPacketInfo(packet_3, tsPacketInfo);
    std::cout << tsPacketInfo.toString() << std::endl;
    return 0;*/

    for (count = 0;; ++count)
    {

        unsigned char packet[TS_PACKET_SIZE];
        // SYNC
        // Check for the sync byte. When found start a new ts-packet parser...
        char b;

        b = getchar();
        while (b != TS_PACKET_SYNC_BYTE)
        {
            b = getchar();
            if (b == EOF)
            {
                std::cout << "End Of File..." << std::endl;
                std::cout << "Found " << count << " ts-packets." << std::endl;
                std::cout << "Found Adaptation Field packets:" << countAdaptPacket << " ts-packets."
                          << std::endl;

                std::cout << "Statistics\n";
                for (auto& pidStat : tsDemux.getTsStatistics().mPidStatistics)
                {
                    if (std::count(g_Options["info"].begin(), g_Options["info"].end(), pidStat.first) == 0)
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
                }
                return EXIT_SUCCESS;
            }
        }

        // TS Packet start
        packet[0] = b;

        // Read TS Packet from stdin
        size_t res =
        fread(packet + 1, 1, TS_PACKET_SIZE - 1, stdin); // Copy only packet-size - sync byte
        (void)res;

        tsDemux.demux(packet);
        if (g_SPPID != 0u)
        {
            // std::cout << "Single Program Transport Stream PID: " << g_SPPID << std::endl;
            tsDemux.addPsiPid(g_SPPID, std::bind(&PMTCallback, std::placeholders::_1));
        }

        for (auto pid : g_ESPIDS)
        {
            tsDemux.addPesPid(pid, std::bind(&PESCallback, std::placeholders::_1, std::placeholders::_2));
        }
        g_ESPIDS.clear();

    } // for loop
}
