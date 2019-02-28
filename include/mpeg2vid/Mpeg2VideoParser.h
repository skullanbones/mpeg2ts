#pragma once

#include <bits/stdint-uintn.h>  // for uint8_t, uint64_t                                                                                                                                                                                                      
#include <cstdint>              // for uint8_t
#include <map>                  // for map, _Rb_tree_const_iterator
#include <string>               // for string, basic_string
#include <utility>              // for pair
#include <vector>               // for vector

/// Project files
#include "EsParser.h"           // for EsParser
#include "GetBits.h"            // for GetBits

namespace mpeg2
{

enum class Mpeg2Type
{
    Info,
    SliceCode,
    SequenceHeader
};

struct EsInfoMpeg2PictureSliceCode
{
    uint64_t picType{ 0 }; // I, B, P
};

struct EsInfoMpeg2SequenceHeader
{
    int width{ 0 }, height{ 0 };
    std::string aspect{ "" };
    std::string framerate{ "" };
};

struct EsInfoMpeg2
{
    Mpeg2Type type;
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
        : EsParser({ 0x00, 0x00, 0x01 })
    {
    }

    virtual ~Mpeg2VideoEsParser() = default;

    /// @brief Parses a binary buffer containing codec data like H262 or H264 and
    /// let the specialization analyze the results.
    /// @param buf The binary data to parse
    std::vector<EsInfoMpeg2> parse(const std::vector<uint8_t>& buf);

    /// @brief Analyze the content on data after startcodes.
    std::vector<EsInfoMpeg2> analyze();

    static std::string toString(Mpeg2Type e);

private:
    static std::map<uint8_t, std::string> AspectToString;
    static std::map<uint8_t, std::string> FrameRateToString;
};


inline std::string Mpeg2VideoEsParser::toString(Mpeg2Type e)
{
    const std::map<Mpeg2Type, std::string> MyEnumStrings{ { Mpeg2Type::Info, "Info" },
                                                          { Mpeg2Type::SliceCode, "SliceCode" },
                                                          { Mpeg2Type::SequenceHeader,
                                                            "SequenceHeader" } };
    auto it = MyEnumStrings.find(e);
    return it == MyEnumStrings.end() ? "Out of range" : it->second;
}
}
