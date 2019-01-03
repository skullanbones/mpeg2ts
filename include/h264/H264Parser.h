#pragma once

#include <map>
#include <vector>

/// Project files
#include "EsParser.h"
#include "GetBits.h"
#include "mpeg2vid/Mpeg2VideoParser.h"

namespace h264
{

struct EsInfoH264 : public ::EsInfo
{
    uint64_t nalUnitType;
    std::string msg;
};

struct EsInfoH264SliceHeader : public EsInfoH264
{
    int sliceType;
    std::string sliceTypeStr;
    int ppsId;
    bool field; // or frame
    bool top;   // or bottom
    int frame_num;
};

struct EsInfoH264SequenceParameterSet : public EsInfoH264
{
    int profileIdc;
    int levelIdc;
    int spsId;
    int lumaBits;
    int chromaBits;
    int numRefPics;
    int width;
    int height;
};

struct EsInfoH264PictureParameterSet : public EsInfoH264
{
    int ppsId;
    int spsId;
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

    std::vector<std::shared_ptr<EsInfo>> analyze() override;

    std::string seipayloadTypeToString(uint64_t payloadType);

    uint64_t getBitsDecodeUGolomb();
    void scaling_list(uint8_t* scalingList, std::size_t sizeOfScalingList);
    std::shared_ptr<EsInfoH264SequenceParameterSet> seq_parameter_set_rbsp(int nal_unit_type);
    std::shared_ptr<EsInfoH264PictureParameterSet> pic_parameter_set_rbsp(int nal_unit_type);
    std::shared_ptr<EsInfoH264SliceHeader> slice_header(int nal_unit_type);
    void parse_vui();
    
private:
    // sps data
    uint8_t log2_max_frame_num_minus4;
    uint64_t separate_colour_plane_flag;
    uint64_t frame_mbs_only_flag;
};

}