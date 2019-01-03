#pragma once

#include <map>
#include <vector>

/// Project files
#include "EsParser.h"
#include "GetBits.h"

namespace mpeg2
{


struct EsInfoMpeg2PictureSliceCode
{
    uint64_t picType{ 0 }; // I, B, P
};

struct EsInfoMpeg2SequenceHeader
{
    int width{ 0 }, height { 0 };
    std::string aspect { 0 };
    std::string framerate { 0 };
};

struct EsInfoMpeg2
{
    int picture{ 0 }; // slice
    std::string msg{ "" };
    EsInfoMpeg2PictureSliceCode slice;
    EsInfoMpeg2SequenceHeader sequence;
};


class Mpeg2VideoEsParser : public GetBits, public EsParser
{
public:
    Mpeg2VideoEsParser(const Mpeg2VideoEsParser& arg) = delete;
    Mpeg2VideoEsParser& operator=(const Mpeg2VideoEsParser& arg) = delete;

    Mpeg2VideoEsParser()
        : EsParser({0x00, 0x00, 0x01})
    {
    }
    
    virtual ~Mpeg2VideoEsParser() = default;

    void analyze() override;

    std::vector<EsInfoMpeg2> getMpeg2Info();
    void clearInfo();

    private : static std::map<uint8_t, std::string> AspectToString;
    static std::map<uint8_t, std::string> FrameRateToString;

    std::vector<EsInfoMpeg2> m_infos;
};

inline std::vector<EsInfoMpeg2> Mpeg2VideoEsParser::getMpeg2Info()
{
    return m_infos;
}

inline void Mpeg2VideoEsParser::clearInfo()
{
    m_infos.clear();
}

}
