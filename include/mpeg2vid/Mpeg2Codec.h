#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace mpeg2 {

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

class Mpeg2VideoEsParser; // forward declaration
class Mpeg2Codec
{
public:
    Mpeg2Codec();
    ~Mpeg2Codec();

    /// @brief Parses a binary buffer containing codec data like H262 or H264 and
    /// let the specialization analyze the results.
    /// @param buf The binary data to parse
    std::vector<EsInfoMpeg2> parse(const std::vector<uint8_t>& buf);

    /// @brief Analyze the content on data after startcodes.
    std::vector<EsInfoMpeg2> analyze();

    std::string toString(Mpeg2Type e);

private:
    // Non copyable
    Mpeg2Codec(const Mpeg2Codec&);
    Mpeg2Codec& operator=(const Mpeg2Codec&);
    // Non movable
    Mpeg2Codec(Mpeg2Codec&&);
    Mpeg2Codec& operator=(Mpeg2Codec&&);
    
    std::unique_ptr<Mpeg2VideoEsParser> mPimpl;
};


} // namespace mpeg2