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
    {
    }
    virtual ~H264EsParser()
    {
    }

    bool analyze() override;
    uint64_t getBitsDecodeUGolomb();
    void scaling_list(uint8_t* scalingList, size_t sizeOfScalingList);
    void seq_parameter_set_rbsp();
    void pic_parameter_set_rbsp();
    void slice_header(bool IdrPicFlag);
    //sps data
    uint64_t log2_max_frame_num_minus4;
    uint64_t separate_colour_plane_flag;
    uint64_t frame_mbs_only_flag;
};
