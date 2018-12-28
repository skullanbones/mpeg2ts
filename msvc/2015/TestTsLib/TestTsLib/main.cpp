// TestTsLib.cpp : Defines the entry point for the console application.
//

#include <public/TsUtilities.h>
#include <public/mpeg2ts.h>

#include <iostream>

using namespace mpeg2ts;

/*!
 * PID: 0
 * PAT packet
 *
 */
const uint8_t pat_packet_1[] =
{ 0x47, 0x40, 0x00, 0x18, 0x00, 0x00, 0xb0, 0x0d, 0x00, 0x01, 0xc1, 0x00, 0x00, 0x00, 0x01, 0xe2,
  0x56, 0xf8, 0x03, 0xe7, 0x1b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

// using namespace tslib;

void PATCallback(PsiTable* table, uint16_t pid, void* hdl)
{
    std::cout << "came here PATCallback" << '\n';

    PatTable* pat;
    try
    {
        pat = dynamic_cast<PatTable*>(table);
    }
    catch (std::exception& ex)
    {
        std::cerr << "ERROR: dynamic_cast ex: " << ex.what();
        return;
    }

    std::cout << "Got PAT callback with PMT PID: " << pat->programs.at(0).program_map_PID << '\n';
}

int main()
{
    // TsUtilities High level API
    tsutil::TsUtilities util;
    // util.parseTransportStreamData(pat_packet_1, 188);

    bool success = util.parseTransportFile("../../../../assets/bbc_one.ts");
    if (!success)
    {
        std::cerr << "Could not open file" << '\n';
        system("PAUSE");
        return EXIT_FAILURE;
    }

    PatTable pat = util.getPatTable();
    std::cout << "Got PAT: " << pat << '\n';

    std::vector<uint16_t> pmtPids = util.getPmtPids();

    std::map<uint16_t, PmtTable> pmtTables = util.getPmtTables();

    for (auto pid : pmtPids)
    {
        std::cout << "Got PMT pid: " << pid << '\n';
        for (auto stream : pmtTables[pid].streams)
        {
            std::cout << "Found elementary stream in PMT :" << stream.elementary_PID << '\n';
        }
    }

    for (auto table : pmtTables)
    {
        std::cout << "PMT PID: " << table.first << '\n';
        std::cout << table.second << '\n';
    }

    std::vector<uint16_t> mEsPids = util.getEsPids();
    for (auto esPid : mEsPids)
    {
        std::cout << "Found elementary stream with Pid: " << esPid << '\n';
    }

    std::map<uint16_t, std::vector<PesPacket>> pesPackets = util.getPesPackets();

    std::cout << "Got number of PES packets: " << pesPackets.size() << '\n';

    for (auto& pes : pesPackets)
    {
        std::cout << "Got PES with PID: " << pes.first << '\n';
        std::cout << "Size of PES packets: " << pes.second.size() << '\n';
    }

    // typedef std::map<int, PidStatistic> PidStatisticsType;
    PidStatisticsMap stat = util.getPidStatistics();

    for (auto pid : stat)
    {
        std::cout << "PID: " << pid.first << '\n';
        std::cout << "numberOfCCErrors: " << pid.second.numberOfCCErrors << '\n';
        std::cout << "numberOfMissingDts: " << pid.second.numberOfMissingDts << '\n';
        std::cout << "numberOfTsDiscontinuities: " << pid.second.numberOfTsDiscontinuities << '\n';
    }


    /* Demonstrates non-orthogonality between APIs...
    try
    {
        std::cout << "Size of pmtTables: " << pmtTables.size() << '\n';
        std::cout << "Got PMT with first stream PID: " <<
    pmtTables[pmtPids.at(0)].streams.at(0).elementary_PID << '\n';
    }
    catch (std::exception& e)
    {
        std::cerr << "Got exception..." << e.what() << '\n';
    }*/

    // Low level API
    /*
    mpeg2ts::TsDemuxer demuxer;

    demuxer.addPsiPid(0, std::bind(&PATCallback, std::placeholders::_1, std::placeholders::_2,
    std::placeholders::_3), nullptr);

    demuxer.demux(pat_packet_1);*/

    system("PAUSE");
    return 0;
}
