///
#include <sstream>
/// 3rd-party
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>

/// Project files
#include "mpeg2vid/Mpeg2VideoParser.h"

namespace mpeg2
{



std::vector<std::shared_ptr<EsInfo>> Mpeg2VideoEsParser::analyze()
{
    resetBits(mPicture.data() + 4, mPicture.size() - 4);
    std::ostringstream msg;
    std::vector<std::shared_ptr<EsInfo>> ret;
    auto rete = std::make_shared<EsInfoMpeg2>();
    rete->picture = mPicture[4];
    if (rete->picture == 0 && mPicture.size() > 4)
    {
        auto retel = std::make_shared<EsInfoMpeg2PictureSliceCode>();
        retel->picture = rete->picture;
        skipBits(10 + 8);
        retel->picType = static_cast<int>(getBits(3));
        switch (retel->picType)
        {
        case 1:
            msg << "I";
            break;
        case 2:
            msg << "P";
            break;
        case 3:
            msg << "B";
            break;
        default:
            msg << "forbiden/reserved";
        };
        retel->msg = msg.str();
        ret.push_back(retel);
    }
    else if (rete->picture >= 0x01 && rete->picture <= 0xaf)
    {
        rete->msg = "slice_start_code";
        ret.push_back(rete);
    }
    else if (rete->picture == 0xb0 && rete->picture == 0xb1 && rete->picture == 0xb6)
    {
        rete->msg = "reserved";
        ret.push_back(rete);
    }
    else if (rete->picture == 0xb2)
    {
        rete->msg = "user_data_start_code";
        ret.push_back(rete);
    }
    else if (rete->picture == 0xb3)
    {
        auto retel = std::make_shared<EsInfoMpeg2SequenceHeader>();
        retel->msg = "sequence_header_code ";
        skipBits(8);
        retel->width = static_cast<int>(getBits(12));
        retel->height = static_cast<int>(getBits(12));
        uint8_t aspect_ratio_information = static_cast<uint8_t>(getBits(4));
        uint8_t frame_rate_code = static_cast<uint8_t>(getBits(4));
        retel->aspect = AspectToString[aspect_ratio_information];
        retel->framerate = FrameRateToString[frame_rate_code];
        ret.push_back(retel);
    }
    else if (rete->picture == 0xb4)
    {
        rete->msg = "sequence_error_code";
        ret.push_back(rete);
    }
    else if (rete->picture == 0xb5)
    {
        rete->msg = "extension_start_code";
        ret.push_back(rete);
    }
    else if (rete->picture == 0xb7)
    {
        rete->msg = "sequence_end_code";
        ret.push_back(rete);
    }
    else if (rete->picture == 0xb8)
    {
        rete->msg = "group_start_code";
        ret.push_back(rete);
    }
    else
    {
        rete->msg = "system start code";
        ret.push_back(rete);
    }
    return ret;
}

std::map<uint8_t, std::string> Mpeg2VideoEsParser::AspectToString =
{ { 0, "forbiden" }, { 1, "square" },   { 2, "3x4" },      { 3, "9x16" },
  { 4, "1x121" },    { 5, "reserved" }, { 6, "reserved" }, { 7, "reserved" } };

std::map<uint8_t, std::string> Mpeg2VideoEsParser::FrameRateToString =
{ { 0, "forbiden" },  { 1, "23.97" },     { 2, "24" },        { 3, "25" },
  { 4, "29.97" },     { 5, "30" },        { 6, "50" },        { 7, "59.94" },
  { 8, "60" },        { 9, "reserved" },  { 10, "reserved" }, { 11, "reserved" },
  { 12, "reserved" }, { 13, "reserved" }, { 14, "reserved" }, { 15, "reserved" } };

}
