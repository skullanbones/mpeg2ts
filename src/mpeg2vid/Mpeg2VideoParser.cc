///
#include <sstream>
/// 3rd-party
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>

/// Project files
#include <mpeg2vid/Mpeg2VideoParser.h>

std::list<std::shared_ptr<EsInfo>> Mpeg2VideoEsParser::operator()(const uint8_t* from, std::size_t length)
{
    auto tmpLast =
    std::vector<uint8_t>(from + length - std::min(length, static_cast<std::size_t>(3)), from + length);
    auto copyFrom = from;
    auto end = from + length;
    while (length > 0)
    {
        auto onePosition = getFirstOne(from, length);
        auto startCodeFound = false;
        if (onePosition == from)
        {
            if (last.size() >= 3 && last[2] == 0 && last[1] == 0 && last[0] == 0)
            {
                startCodeFound = true;
            }
        }
        else if (onePosition == from + 1)
        {
            if (last.size() >= 2 && last[1] == 0 && last[0] == 0 && *(onePosition - 1) == 0)
            {
                startCodeFound = true;
            }
        }
        else if (onePosition == from + 2)
        {
            if (last.size() >= 1 && last[0] == 0 && *(onePosition - 1) == 0 && *(onePosition - 2) == 0)
            {
                startCodeFound = true;
            }
        }
        else
        {
            if ((onePosition != from + length) && *(onePosition - 1) == 0 &&
                *(onePosition - 2) == 0 && *(onePosition - 3) == 0)
            {
                startCodeFound = true;
            }
        }
        if (startCodeFound)
        {
            ++foundStartCodes;
            const uint8_t* r = onePosition + 1;
            mPicture.insert(mPicture.end(), copyFrom, r);
            if (!mPicture.empty())
            {
                analyze();
            }
            mPicture.clear();
            copyFrom = onePosition + 1;
        }
        int diff = (onePosition - from);
        length -= diff;
        from = onePosition + 1;
    }

    last = tmpLast;

    if (copyFrom < end)
    {
        mPicture.insert(mPicture.end(), copyFrom, end);
    }
 
    return std::list<std::shared_ptr<EsInfo>>();
}

std::list<std::shared_ptr<EsInfo>> Mpeg2VideoEsParser::analyze()
{
    resetBits(mPicture.data(), mPicture.size());
    std::ostringstream msg;
    if (mPicture[0] == 0 && mPicture.size() > 4) // TODO: 4 ?
    {
        msg << "picture_start_code ";
        skipBits(10 + 8);
        auto picType = getBits(3);
        switch (picType)
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
        LOGD << msg.str();
    }
    else if (mPicture[0] >= 0x01 && mPicture[0] <= 0xaf)
    {
        LOGD << "slice_start_code";
    }
    else if (mPicture[0] == 0xb0 && mPicture[0] == 0xb1 && mPicture[0] == 0xb6)
    {
        LOGD << "reserved";
    }
    else if (mPicture[0] == 0xb2)
    {
        LOGD << "user_data_start_code";
    }
    else if (mPicture[0] == 0xb3)
    {
        msg << "sequence_header_code ";
        skipBits(8);
        auto horizontal_size_value = getBits(12);
        auto vertical_size_value = getBits(12);
        auto aspect_ratio_information = getBits(4);
        auto frame_rate_code = getBits(4);
        msg << "size " << horizontal_size_value << " x " << vertical_size_value;
        msg << ", aspect " << AspectToString[aspect_ratio_information];
        msg << ", frame rate " << FrameRateToString[frame_rate_code];
        LOGD << msg.str();
    }
    else if (mPicture[0] == 0xb4)
    {
        LOGD << "sequence_error_code";
    }
    else if (mPicture[0] == 0xb5)
    {
        LOGD << "extension_start_code";
    }
    else if (mPicture[0] == 0xb7)
    {
        LOGD << "sequence_end_code";
    }
    else if (mPicture[0] == 0xb8)
    {
        LOGD << "group_start_code";
    }
    else
    {
        LOGD << "system start code";
    }
    return std::list<std::shared_ptr<EsInfo>>();
}

std::map<uint8_t, std::string> Mpeg2VideoEsParser::AspectToString =
{ { 0, "forbiden" }, { 1, "square" },   { 2, "3x4" },      { 3, "9x16" },
  { 4, "1x121" },    { 5, "reserved" }, { 6, "reserved" }, { 7, "reserved" } };

std::map<uint8_t, std::string> Mpeg2VideoEsParser::FrameRateToString =
{ { 0, "forbiden" },  { 1, "23.97" },     { 2, "24" },        { 3, "25" },
  { 4, "29.97" },     { 5, "30" },        { 6, "50" },        { 7, "59.94" },
  { 8, "60" },        { 9, "reserved" },  { 10, "reserved" }, { 11, "reserved" },
  { 12, "reserved" }, { 13, "reserved" }, { 14, "reserved" }, { 15, "reserved" } };
