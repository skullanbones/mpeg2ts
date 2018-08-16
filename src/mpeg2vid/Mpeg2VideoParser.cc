/// 3rd-party
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>

/// Project files
#include "mpeg2vid/Mpeg2VideoParser.h"

bool Mpeg2VideoEsParser::operator()(const uint8_t* from, ssize_t length)
{
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
            if ((onePosition != from + length) && *(onePosition - 1] == 0 && *(onePosition - 2) == 0 && *(onePosition - 3) == 0)
            {
                startCodeFound = true;
            }
        }

        from = onePosition + 1;
        length -= onePosition - from - 1;


        if (onePosition < 0)
        {
            last = std::vector(from + length - std::min(length, 3), from + length);
            length = 0;
            break;
        }
        // TODO: process start code at onePosition;
        LOGD << "start code " << from[onePosition] << " left in buffer " << ;

        from
    }

    return true;
}
