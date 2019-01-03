#pragma once

#include <map>
#include <vector>

/// Project files
#include "EsParser.h"
#include "GetBits.h"
#include "mpeg2vid/Mpeg2VideoParser.h"

namespace h264
{

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

    void analyze() override;

    std::string seipayloadTypeToString(uint64_t payloadType);

    void slice_header(int nal_unit_type, EsInfoH264& info);
    uint64_t getBitsDecodeUGolomb();
    void scaling_list(uint8_t* scalingList, std::size_t sizeOfScalingList);
    void seq_parameter_set_rbsp(int nal_unit_type, EsInfoH264& info);
    void pic_parameter_set_rbsp(int nal_unit_type, EsInfoH264& info);
    
    void parse_vui();

    std::vector<EsInfoH264> getInfo();
    void clearInfo();

private:
    // sps data
    uint8_t log2_max_frame_num_minus4;
    uint64_t separate_colour_plane_flag;
    uint64_t frame_mbs_only_flag;
    std::vector<EsInfoH264> m_infos;
};

inline std::vector<EsInfoH264> H264EsParser::getInfo()
{
    return m_infos;
}

inline void H264EsParser::clearInfo()
{
    m_infos.clear();
}

}