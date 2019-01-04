#pragma once

#include <map>
#include <vector>

/// Project files
#include "EsParser.h"
#include "GetBits.h"
#include "mpeg2vid/Mpeg2VideoParser.h"

namespace h264
{

enum class H264InfoType
{
    Info,
    SliceHeader,
    SequenceParameterSet,
    PictureParameterSet
};


struct EsInfoH264SliceHeader
{
    int sliceType { 0 };
    std::string sliceTypeStr { "" };
    int ppsId { 0 };
    bool field { false }; // or frame
    bool top { false };   // or bottom
    int frame_num { 0 };
};

struct EsInfoH264SequenceParameterSet
{
    int profileIdc { 0 };
    int levelIdc { 0 };
    int spsId { 0 };
    int lumaBits { 0 };
    int chromaBits { 0 };
    int numRefPics { 0 };
    int width { 0 };
    int height { 0 };
};

struct EsInfoH264PictureParameterSet
{
    int ppsId { 0 };
    int spsId { 0 };
};

struct EsInfoH264
{
    H264InfoType type;
    uint64_t nalUnitType { 0 };
    std::string msg { "" };
    EsInfoH264SliceHeader slice;
    EsInfoH264SequenceParameterSet sps;
    EsInfoH264PictureParameterSet pps;
};

class H264EsParser : public EsParser, public GetBits
{
public:
    H264EsParser(const H264EsParser& arg) = delete;
    H264EsParser& operator=(const H264EsParser& arg) = delete;
    H264EsParser()
        : EsParser({0x00, 0x00, 0x00, 0x01})
        , log2_max_frame_num_minus4{ 0 }
        , separate_colour_plane_flag{ 0 }
        , frame_mbs_only_flag{ 0 }
    {
    }

    virtual ~H264EsParser() = default;

    /// @brief Parses a binary buffer containing codec data like H262 or H264 and
    /// let the specialization analyze the results.
    /// @param buf The binary data to parse
    std::vector<EsInfoH264> parse(const std::vector<uint8_t>& buf);

    /// @brief Analyze the content on data after startcodes.
    std::vector<EsInfoH264> analyze();

    std::string seipayloadTypeToString(uint64_t payloadType);

    void slice_header(int nal_unit_type, EsInfoH264& info);
    uint64_t getBitsDecodeUGolomb();
    void scaling_list(uint8_t* scalingList, std::size_t sizeOfScalingList);
    void seq_parameter_set_rbsp(int nal_unit_type, EsInfoH264& info);
    void pic_parameter_set_rbsp(int nal_unit_type, EsInfoH264& info);
    
    void parse_vui();

    std::vector<EsInfoH264> getInfo();
    void clearInfo();

    static std::string toString (H264InfoType e);

private:
    // sps data
    uint8_t log2_max_frame_num_minus4;
    uint64_t separate_colour_plane_flag;
    uint64_t frame_mbs_only_flag;
};

inline std::string H264EsParser::toString (H264InfoType e)
{
    const std::map<H264InfoType, std::string> MyEnumStrings {
        { H264InfoType::Info, "Info" },
        { H264InfoType::SliceHeader, "SliceHeader" },
        { H264InfoType::SequenceParameterSet, "SequenceParameterSet" },
        { H264InfoType::PictureParameterSet, "PictureParameterSet" }
    };
    auto   it  = MyEnumStrings.find(e);
    return it == MyEnumStrings.end() ? "Out of range" : it->second;
}

}