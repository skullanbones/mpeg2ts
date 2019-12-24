// TestTsLib.cpp : Defines the entry point for the console application.
//

#include "TsUtilities.h"
#include "mpeg2ts.h"

#include "Mpeg2Codec.h"
#include "H264Codec.h"

#include <iostream>

// 3rd-party
#include "plog/Log.h"

namespace tsutil {

void handleVideoCallback(const std::vector<uint8_t>& data, int streamType)
{
    try
    {
        tsutil::VideoMediaInfo videoInfo;
        switch (streamType)
        {
        case mpeg2ts::STREAMTYPE_VIDEO_MPEG2:
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
                    // LOGD << "mpeg2 picture type: " << info.slice.picType << " " << info.msg;
                }
                else if (info.type == mpeg2::Mpeg2Type::SequenceHeader)
                {
                    videoInfo.width = info.sequence.width;
                    videoInfo.height = info.sequence.height;
                    videoInfo.frameRate = info.sequence.framerate;
                    videoInfo.aspect = info.sequence.aspect;
                }
            }
            break;
        } // STREAMTYPE_VIDEO_MPEG2
        case mpeg2ts::STREAMTYPE_VIDEO_H264:
        {
            videoInfo.codec = VideoCodecType::H264;
            videoInfo.mediaType = MediaType::Video;
            // videoInfo.PID = a_pid;

            h264::H264Codec h264Parser;
            std::vector<h264::EsInfoH264> ret = h264Parser.parse(data);

            for (const h264::EsInfoH264& info : ret)
            {
                // LOGD << "nal: " << h264::H264EsParser::toString(info.nalUnitType) << " " << info.msg;
                if (info.type == h264::H264InfoType::SliceHeader)
                {
                    LOGD << info.slice.sliceTypeStr << ", pps id: " << info.slice.ppsId;
                    if (info.slice.field)
                    {
                        LOGD << "field encoded: " << (info.slice.top ? " top" : "bottom");
                    }
                    else
                    {
                        LOGD << "frame encoded";
                    }
                }
                else if (info.type == h264::H264InfoType::SequenceParameterSet)
                {
                    videoInfo.width = info.sps.width;
                    videoInfo.height = info.sps.height;
                    videoInfo.lumaBits = info.sps.lumaBits;
                    videoInfo.chromaBits = info.sps.chromaBits;
                    videoInfo.numRefPics = info.sps.numRefPics;

                    LOGD << "sps id: " << info.sps.spsId << ", luma bits: " <<
                    info.sps.lumaBits
                         << ", chroma bits: " << info.sps.chromaBits << ", width: "
                         << info.sps.width
                         << " x " << info.sps.height << ", ref pic: " <<
                         info.sps.numRefPics;
                }
                else if (info.type == h264::H264InfoType::PictureParameterSet)
                {
                    LOGD << "sps id: " << info.pps.spsId << "pps id: " <<
                    info.pps.ppsId;
                }
            }
            break;
        } // STREAMTYPE_VIDEO_H264
        default:
        {
            LOGD << "Unknown codec!";
        }
        }
    }
    catch (const std::out_of_range&)
    {
        LOGE << "No parser for stream type " << mpeg2ts::StreamTypeToString[streamType];
    }
}


} // namespace tsutil

int main(int argc, char *argv[])
{
    std::string asset;
    if (argc != 2)
    {
        std::cerr << "Need asset argument! example: ./sample_tsutilities.exe myasset.ts" << '\n';
        return EXIT_FAILURE;
    }
    else {
        std::cout << "Using input asset: " << argv[1] << '\n';
        asset = argv[1];
    }

    tsutil::TsUtilities util; // TsUtilities High level API

    util.addVideoCallback(
        [&](const std::vector<uint8_t>& a_data, int a_streamType) {
            tsutil::handleVideoCallback(a_data, a_streamType);
        }
    );

    bool success = util.parseTransportFile(argv[1]);
    LOGD << "Starting parser of file";
    if (!success)
    {
        LOGE << "Could not open file";
        return EXIT_FAILURE;
    }

    mpeg2ts::PatTable pat = util.getPatTable();
    LOGD << "Got PAT: " << pat;

    std::vector<uint16_t> pmtPids = util.getPmtPids();

    std::map<int, mpeg2ts::PmtTable> pmtTables = util.getPmtTables();

    for (auto pid : pmtPids)
    {
        std::cout << "Got PMT pid: " << pid;
        for (auto stream : pmtTables[pid].streams)
        {
            LOGD << "Found elementary stream in PMT :" << stream.elementary_PID;
        }
    }

    for (auto table : pmtTables)
    {
        LOGD << "PMT PID: " << table.first;
        LOGD << table.second;
    }

    std::vector<uint16_t> mEsPids = util.getEsPids();
    for (auto esPid : mEsPids)
    {
        LOGD << "Found elementary stream with Pid: " << esPid;
    }

    std::map<int, std::vector<mpeg2ts::PesPacket>> pesPackets = util.getPesPackets();

    LOGD << "Got number of PES packets: " << pesPackets.size();

    for (auto& pes : pesPackets)
    {
        LOGD << "Got PES with PID: " << pes.first;
        LOGD << "Size of PES packets: " << pes.second.size();
    }

    mpeg2ts::PidStatisticsMap stat = util.getPidStatistics();

    for (auto pid : stat)
    {
        LOGD << "PID: " << pid.first;
        LOGD << "numberOfCCErrors: " << pid.second.numberOfCCErrors;
        LOGD << "numberOfMissingDts: " << pid.second.numberOfMissingDts;
        LOGD << "numberOfTsDiscontinuities: " << pid.second.numberOfTsDiscontinuities;
    }

    return EXIT_SUCCESS;
}
