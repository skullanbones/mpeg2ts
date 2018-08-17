/// 3rd-party
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>

/// Project files
#include "mpeg2vid/Mpeg2VideoParser.h"

bool Mpeg2VideoEsParser::operator()(const uint8_t* from, ssize_t length)
{
    auto tmpLast = std::vector<uint8_t>(from + length - std::min(length, (ssize_t)3), from + length);
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
        }else if (onePosition == from + 1)
        {
            if (last.size() >= 2 && last[1] == 0 && last[0] == 0 && *(onePosition - 1) == 0)
            {
                startCodeFound = true;
            }
        }else if (onePosition == from + 2)
        {
            if (last.size() >= 1 && last[0] == 0 && *(onePosition - 1) == 0 && *(onePosition - 2) == 0)
            {
                startCodeFound = true;
            }
        }else
        {
            if ((onePosition != from + length) && *(onePosition - 1) == 0 && *(onePosition - 2) == 0 && *(onePosition - 3) == 0)
            {
                startCodeFound = true;
            }
        }
        if (startCodeFound)
        {
            ++foundStartCodes;
            const uint8_t* r = onePosition + 1;
            mPicture.insert(mPicture.end(), copyFrom, r);
            /*for (auto x : mPicture)
            {
                std::cout << std::hex << (int)x << " ";
            }*/
            if (!mPicture.empty())
            {
                analyze();
            }
            mPicture.clear();
            copyFrom = onePosition + 1;

        }
        length -= (onePosition - from) + 1;
        from = onePosition + 1;
    }
    
    last = tmpLast;

    if (copyFrom < end)
    {
        mPicture.insert(mPicture.end(), copyFrom, end);
    }

    return true;
}

bool Mpeg2VideoEsParser::analyze()
{
            resetBits(mPicture.data(), mPicture.size());
            if (mPicture[0] == 0 && mPicture.size() > 4) //TODO: 4 ?
            {
                std::cout << "picture_start_code ";
                skipBits(10 + 8);
                auto picType = getBits(3);
                switch (picType)
                {
                    case 1: std::cout << "I";
                        break;
                    case 2: std::cout << "P";
                        break;
                    case 3: std::cout << "B";
                        break;
                    default:
                            std::cout << "forbiden/reserved";

                };
            }else if (mPicture[0] >= 0x01 && mPicture[0] <= 0xaf)
            {
                std::cout << "slice_start_code";
            }else if (mPicture[0] == 0xb0 && mPicture[0] == 0xb1 && mPicture[0] == 0xb6)
            {
                std::cout << "reserved";
            }else if (mPicture[0] == 0xb2)
            {
                std::cout << "user_data_start_code";
            }else if (mPicture[0] == 0xb3)
            {
                std::cout << "sequence_header_code ";
                skipBits(8);
                auto horizontal_size_value = getBits(12);
                auto vertical_size_value = getBits(12);
                auto aspect_ratio_information = getBits(4);
                auto frame_rate_code = getBits(4);
                std::cout << "size " << horizontal_size_value << " x " << vertical_size_value;
                std::cout << ", aspect " << AspectToString[aspect_ratio_information];
                std::cout << ", frame rate " << FrameRateToString[frame_rate_code];
            }else if (mPicture[0] == 0xb4)
            {
                std::cout << "sequence_error_code";
            }else if (mPicture[0] == 0xb5)
            {
                std::cout << "extension_start_code";
            }else if (mPicture[0] == 0xb7)
            {
                std::cout << "sequence_end_code";
            }else if (mPicture[0] == 0xb8)
            {
                std::cout << "group_start_code";
            }else
            {
                std::cout << "system start code";
            }
            std::cout << "\n";
            return true;
}

std::map<uint8_t, std::string> Mpeg2VideoEsParser::AspectToString = {{0,"forbiden"},
{1,"square"},
{2,"3x4"},
{3,"9x16"},
{4,"1x121"},
{5,"reserved"},
{6,"reserved"},
{7,"reserved"}
};

std::map<uint8_t, std::string> Mpeg2VideoEsParser::FrameRateToString = {{0,"forbiden"},
{1,"23.97"},
{2,"24"},
{3,"25"},
{4,"29.97"},
{5,"30"},
{6,"50"},
{7,"59.94"},
{8,"60"},
{9,"reserved"},
{10,"reserved"},
{11,"reserved"},
{12,"reserved"},
{13,"reserved"},
{14,"reserved"},
{15,"reserved"}
};
