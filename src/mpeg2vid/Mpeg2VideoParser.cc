///
#include <sstream>
/// 3rd-party
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>

/// Project files
#include "mpeg2vid/Mpeg2VideoParser.h"

namespace mpeg2
{



void Mpeg2VideoEsParser::analyze()
{
    resetBits(mPicture.data(), mPicture.size());
    std::ostringstream msg;

    EsInfoMpeg2 info;
    info.type = Mpeg2Type::Info;
    info.picture = mPicture[0];
    if (info.picture == 0 && mPicture.size() > 4)
    {
        info.type = Mpeg2Type::SliceCode;
        skipBits(10 + 8);
        info.slice.picType = static_cast<int>(getBits(3));
        switch (info.slice.picType)
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
        info.msg = msg.str();
    }
    else if (info.picture >= 0x01 && info.picture <= 0xaf)
    {
        info.msg = "slice_start_code";
    }
    else if (info.picture == 0xb0 && info.picture == 0xb1 && info.picture == 0xb6)
    {
        info.msg = "reserved";
    }
    else if (info.picture == 0xb2)
    {
        info.msg = "user_data_start_code";
    }
    else if (info.picture == 0xb3)
    {
        info.type = Mpeg2Type::SequenceHeader;
        info.msg = "sequence_header_code ";
        skipBits(8);
        info.sequence.width = static_cast<int>(getBits(12));
        info.sequence.height = static_cast<int>(getBits(12));
        uint8_t aspect_ratio_information = static_cast<uint8_t>(getBits(4));
        uint8_t frame_rate_code = static_cast<uint8_t>(getBits(4));
        info.sequence.aspect = AspectToString[aspect_ratio_information];
        info.sequence.framerate = FrameRateToString[frame_rate_code];
    }
    else if (info.picture == 0xb4)
    {
        info.msg = "sequence_error_code";
    }
    else if (info.picture == 0xb5)
    {
        info.msg = "extension_start_code";
    }
    else if (info.picture == 0xb7)
    {
        info.msg = "sequence_end_code";
    }
    else if (info.picture == 0xb8)
    {
        info.msg = "group_start_code";
    }
    else
    {
        info.msg = "system start code";
    }
    m_infos.push_back(std::move(info));
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
