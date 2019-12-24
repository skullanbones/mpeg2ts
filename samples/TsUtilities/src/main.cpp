// TestTsLib.cpp : Defines the entry point for the console application.
//

#include "TsUtilities.h"
#include "mpeg2ts.h"

#include "Mpeg2Codec.h"
#include "H264Codec.h"

#include <iostream>

namespace tsutil {

void handleVideoCallback(const std::vector<uint8_t>& data, int streamType)
{
    try
    {
        tsutil::VideoMediaInfo videoInfo;
        if (streamType == mpeg2ts::STREAMTYPE_VIDEO_MPEG2)
        {
            videoInfo.codec = VideoCodecType::MPEG2;
            videoInfo.mediaType = MediaType::Video;

            mpeg2::Mpeg2Codec mpeg2Parser;
            std::vector<mpeg2::EsInfoMpeg2> ret = mpeg2Parser.parse(data);

            for (const mpeg2::EsInfoMpeg2& info : ret)
            {
                // LOGD << "mpeg2 picture: " << info.picture << " " << info.msg;
                if (info.type == mpeg2::Mpeg2Type::SliceCode)
                {
                    videoInfo.picType = info.slice.picType;
                    // LOGD << "mpeg2 picture type: " << info.slice.picType << " " <<
                    // info.msg;
                }
                else if (info.type == mpeg2::Mpeg2Type::SequenceHeader)
                {
                    videoInfo.width = info.sequence.width;
                    videoInfo.height = info.sequence.height;
                    videoInfo.frameRate = info.sequence.framerate;
                    videoInfo.aspect = info.sequence.aspect;
                }
            }
        } // STREAMTYPE_VIDEO_MPEG2

        if (streamType == mpeg2ts::STREAMTYPE_VIDEO_H264)
        {
            videoInfo.codec = VideoCodecType::H264;
            videoInfo.mediaType = MediaType::Video;
            // videoInfo.PID = a_pid;

            h264::H264Codec h264Parser;
            std::vector<h264::EsInfoH264> ret = h264Parser.parse(data);

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
                    videoInfo.width = info.sps.width;
                    videoInfo.height = info.sps.height;
                    videoInfo.lumaBits = info.sps.lumaBits;
                    videoInfo.chromaBits = info.sps.chromaBits;
                    videoInfo.numRefPics = info.sps.numRefPics;

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
        std::cerr << "No parser for stream type " << mpeg2ts::StreamTypeToString[streamType];
    }
}

}

int main(int argc, char *argv[])
{
    tsutil::TsUtilities util; // TsUtilities High level API

    util.addVideoCallback(
        [&](const std::vector<uint8_t>& a_data, int a_streamType) {
            printf("Came to ballback... %d\n", a_streamType);

            tsutil::handleVideoCallback(a_data, a_streamType);
        }
    );

    std::string asset;

    if (argc != 2)
    {
        std::cerr << "Need asset argument! example: ./sample_tsutilities.exe myasset.ts" << '\n';
        asset = "../../../assets/bbc_one.ts";
    }
    else {
        std::cout << "Using input asset: " << argv[1] << '\n';
        asset = argv[1];
    }

    bool success = util.parseTransportFile(argv[1]);
    if (!success)
    {
        std::cerr << "Could not open file" << '\n';
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

    mpeg2ts::PidStatisticsMap stat = util.getPidStatistics();

    for (auto pid : stat)
    {
        std::cout << "PID: " << pid.first << '\n';
        std::cout << "numberOfCCErrors: " << pid.second.numberOfCCErrors << '\n';
        std::cout << "numberOfMissingDts: " << pid.second.numberOfMissingDts << '\n';
        std::cout << "numberOfTsDiscontinuities: " << pid.second.numberOfTsDiscontinuities << '\n';
    }
    
    return EXIT_SUCCESS;
}
