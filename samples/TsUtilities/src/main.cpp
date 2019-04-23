// TestTsLib.cpp : Defines the entry point for the console application.
//

#include "TsUtilities.h"
#include "mpeg2ts.h"

#include <iostream>


int main(int argc, char *argv[])
{
    // TsUtilities High level API
    tsutil::TsUtilities util;

    std::string asset;

    if (argc != 2)
    {
        std::cerr << "Need asset argument! example: ./sample_tsutilities.exe myasset.ts" << '\n';
       // system("PAUSE");
        asset = "../../../assets/bbc_one.ts";
    }
    else {
        std::cout << "Using input asset: " << argv[1] << '\n';
        asset = argv[1];
    }

    bool success = util.parseTransportFile("../../../assets/bbc_one.ts");
    if (!success)
    {
        std::cerr << "Could not open file" << '\n';
//        system("PAUSE");
        return EXIT_FAILURE;
    }

    mpeg2ts::PatTable pat = util.getPatTable();
    std::cout << "Got PAT: " << pat << '\n';

    std::vector<uint16_t> pmtPids = util.getPmtPids();

    std::map<int, mpeg2ts::PmtTable> pmtTables = util.getPmtTables();

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

    std::map<int, std::vector<mpeg2ts::PesPacket>> pesPackets = util.getPesPackets();

    std::cout << "Got number of PES packets: " << pesPackets.size() << '\n';

    for (auto& pes : pesPackets)
    {
        std::cout << "Got PES with PID: " << pes.first << '\n';
        std::cout << "Size of PES packets: " << pes.second.size() << '\n';
    }

    // typedef std::map<int, PidStatistic> PidStatisticsType;
    mpeg2ts::PidStatisticsMap stat = util.getPidStatistics();

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

    int systemRet = system("PAUSE");
    if (systemRet < 0)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
