/// 3rd-party
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>

/// Project files
#include "mpeg2vid/Mpeg2VideoParser.h"

bool Mpeg2VideoEsParser::operator()(const uint8_t* from, ssize_t length)
{
    auto tmpLast = std::vector<uint8_t>(from + length - std::min(length, (ssize_t)3), from + length);
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
        }
        length -= (onePosition - from) + 1;
        from = onePosition + 1;
    }
    last = tmpLast;

    return true;
}
