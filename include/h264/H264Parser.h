#pragma once

#include <vector>
#include <map>

/// Project files
#include "EsParser.h"
#include "GetBits.h"
#include "mpeg2vid/Mpeg2VideoParser.h"

class H264EsParser : public Mpeg2VideoEsParser
{
public:
    H264EsParser()
        : foundStartCodes{0}

    {
    }
    virtual ~H264EsParser()
    {
    }

    virtual bool operator()(const uint8_t* from, ssize_t length);
    bool analyze();

    std::vector<uint8_t> last;
    int foundStartCodes;
    std::vector<uint8_t> mPicture;
};
