/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */
#include <cstdlib>
#include <iostream>
#include <stdint.h>
#include <getopt.h>
#include <unistd.h>
#include <algorithm> 
#include <fstream>

#include "TsDemuxer.h"
#include "TsPacketInfo.h"
#include "TsParser.h"
#include "TsStandards.h"

#include <type_traits>
#include <map>
#include <string>

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
std::map<std::string, std::vector<int> > g_Options;
struct option longOptions[] = {
            {"write", 1, 0, 'w' },
            {"info",  1, 0, 'i' },
            {"level", 1, 0, 'l' },
            {0,       0, 0,  0 }
        };

void TsCallback(unsigned char packet, TsPacketInfo tsPacketInfo)
{
    (void)packet;
    std::cout << "demuxed TS packet \n" << tsPacketInfo;
}

void PATCallback(PsiTable* table)
{
    std::cout << "demuxed PAT table \n";

    PatTable* pat = static_cast<PatTable*>(table);
    if (std::count(g_Options["info"].begin(), g_Options["info"].end(), 0))
    {
        std::cout << *pat << std::endl;
    }
    
    g_SPPID = pat->programs[0].program_map_PID; // Assume SPTS
}

void PMTCallback(PsiTable* table)
{
    std::cout << "demuxed PMT table \n";

    PmtTable* pmt = static_cast<PmtTable*>(table);
    if (std::count(g_Options["info"].begin(), g_Options["info"].end(), g_SPPID))
    {
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
    std::cout << "demuxed PES packet on pid " << pid << "\n";
    
    if (std::count(g_Options["info"].begin(), g_Options["info"].end(), pid))
    {
        std::cout << pes << std::endl;
    }
    
    if (std::count(g_Options["write"].begin(), g_Options["write"].end(), pid))
    {
        static std::map<uint16_t, std::ofstream> outFiles;
        auto fit = outFiles.find(pid);
        if (fit == outFiles.end())
        {
            outFiles[pid] = std::ofstream("/tmp/out" + std::to_string(pid) + ".pes", std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        }
        
        std::copy(pes.mPesBuffer.begin(), pes.mPesBuffer.end(), std::ostreambuf_iterator<char>(outFiles[pid]));
        
        std::cout << "Write " << pes.mPesBuffer.size() << " B of PesPacket on pid: " << pid << std::endl;
    }
}

int main(int argc, char** argv)
{
    std::cout << "Staring parser of stdout" << std::endl;
    
    int opt, optInx;
    while ((opt = getopt_long(argc, argv, "", longOptions, &optInx)) != -1){
        g_Options[longOptions[optInx].name].push_back(std::atoi(optarg));
    }

    uint64_t count;

    // Specify input stream
    setvbuf(stdout, NULL, _IOLBF, 0);

    //  char buffer[200*10224*1024];
    //  setbuf(stdin, buffer);

    // unsigned long position = 0;

    TsPacketInfo tsPacketInfo = { 0 };
    TsParser tsParser;

    TsDemuxer tsDemux;
    tsDemux.addPsiPid(TS_PACKET_PID_PAT, std::bind(&PATCallback, std::placeholders::_1));

    //    TsAdaptationFieldHeader fieldHeader;

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
                return EXIT_SUCCESS;
            }
        }


        /*
        while (buffer[position++] != TS_PACKET_SYNC_BYTE) {
          if (buffer[position] == EOF)
             {
               std::cout << "End Of File..." << std::endl;
               std::cout << "Found " << count << " ts-packets." << std::endl;
          std::cout << "Found Adaptation Field packets:" << countAdaptPacket
                << " ts-packets." << std::endl;
               return EXIT_SUCCESS;
             }
        }
        */

        // TS Packet start
        packet[0] = b;

        // Read TS Packet from stdin
        size_t res =
        fread(packet + 1, 1, TS_PACKET_SIZE - 1, stdin); // Copy only packet-size - sync byte
        (void)res;
        // memcpy(packet, &buffer[position], TS_PACKET_SIZE);

        // For debug purpose
        tsParser.parseTsPacketInfo(packet, tsPacketInfo);
        //        std::cout << tsPacketInfo.toString() << std::endl;

        tsDemux.demux(packet);
        if (g_SPPID)
        {
            // std::cout << "Single Program Transport Stream PID: " << g_SPPID << std::endl;
            tsDemux.addPsiPid(g_SPPID, std::bind(&PMTCallback, std::placeholders::_1));
        }

        for (auto pid : g_ESPIDS)
        {
            tsDemux.addPesPid(pid, std::bind(&PESCallback, std::placeholders::_1, std::placeholders::_2));
        }
        g_ESPIDS.clear();

        if (tsPacketInfo.hasAdaptationField)
        {
            //      std::cout << "found packet with adaptation field";
            countAdaptPacket++;

            if (countAdaptPacket == 1)
            {
                for (int i = 0; i < TS_PACKET_SIZE; i++)
                {
                    //	  printf("0x%1x, ", packet[i]);
                }
            }
        }

    } // for loop
}
