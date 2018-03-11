/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */
#include <cstdlib>
#include <iostream>
#include <stdint.h>

#include <unistd.h>

#include "TsDemuxer.h"
#include "TsPacketInfo.h"
#include "TsParser.h"
#include "TsStandards.h"

#include <type_traits>

uint64_t count = 0;
uint64_t countAdaptPacket = 0;
uint32_t g_SPPID = 0; // Single Program PID
uint32_t g_ESPID1 = 0; // Test 1 ES

void TsCallback(unsigned char packet, TsPacketInfo tsPacketInfo)
{
    (void)packet;
    std::cout << "demuxed TS packet \n" << tsPacketInfo;
}

void PATCallback(PsiTable* table)
{
    std::cout << "demuxed PAT table \n" << table->table_id;

    // std::cout << "Got PSI table:" << std::endl << table << std::endl;
    PatTable* pat = static_cast<PatTable*>(table);
    // std::cout << "Got PAT packet:" << std::endl << *pat << std::endl;
    g_SPPID = pat->programs[0].program_map_PID; // Assume SPTS
}

void PMTCallback(PsiTable* table)
{
    std::cout << "demuxed PMT table \n" << table->table_id;

    std::cout << "Got PSI table:" << std::endl << *table << std::endl;
    PmtTable* pmt = static_cast<PmtTable*>(table);
    std::cout << "Got PMT packet:" << std::endl << *pmt << std::endl;

    std::cout << "pmt streams size:" << pmt->streams.size() << std::endl;
    g_ESPID1 = pmt->streams[0].elementary_PID; // Use 1st ES
}

void PESCallback(const PesPacket& pes)
{
    std::cout << "demuxed PES packet \n";
    std::cout << "Got PesPacket start: " << std::endl << pes << std::endl;
}

int main(int argc, char** argv)
{
    std::cout << "Staring parser of stdout" << std::endl;

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

        if (g_ESPID1)
        {
            tsDemux.addPesPid(g_ESPID1, std::bind(&PESCallback, std::placeholders::_1));
        }

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
