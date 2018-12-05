///
#include <sstream>
/// 3rd-party
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>

/// Project files
#include <mpeg2vid/Mpeg2VideoParser.h>

std::vector<std::shared_ptr<EsInfo>> Mpeg2VideoEsParser::operator()(const uint8_t* from, std::size_t length)
{
    std::vector<std::shared_ptr<EsInfo>> ret;
    while (length > 0)
    {
        const uint8_t* onePosition = getFirstOne(from, length);
        auto startCodeFound = false;
        if (onePosition == from)
        {
            if (mPicture.size() >= 2 && mPicture[mPicture.size() - 2] == 0 && mPicture[mPicture.size() - 1] == 0)
            {
                startCodeFound = true;
            }
        }
        else if (onePosition == from + 1)
        {
            if (mPicture.size() >= 1 && mPicture[mPicture.size() - 1] == 0 && *(onePosition - 1) == 0)
            {
                startCodeFound = true;
            }
        }
        else if (onePosition != from + length)
        {
            if (*(onePosition - 2) == 0 && *(onePosition - 1) == 0)
            {
                startCodeFound = true;
            }
        }
        const uint8_t* end = (onePosition != from + length) ? onePosition + 1 : onePosition;
        std::copy(from, end, std::back_inserter(mPicture));
        if (startCodeFound)
        {
            ++foundStartCodes;
            if (mPicture.size() > 4)
            {
                auto vec = analyze();
                for (auto& l : vec)
                {
                    ret.push_back(l);
                }
            }
            mPicture = { 0, 0, 0, 1 };
        }
        size_t diff = (onePosition + 1 - from);
        length = diff > length ? 0 : length - diff;
        from = onePosition + 1;
    }

    return ret;
}

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
        retel->picType = getBits(3);
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
        retel->width = getBits(12);
        retel->height = getBits(12);
        auto aspect_ratio_information = getBits(4);
        auto frame_rate_code = getBits(4);
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
