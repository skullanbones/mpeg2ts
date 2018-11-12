///
#include <sstream>
/// 3rd-party
#include <plog/Log.h>

/// Project files
#include "h264/H264Parser.h"

std::list<std::shared_ptr<EsInfo>> H264EsParser::analyze()
{
    resetBits(mPicture.data(), mPicture.size());
    std::list<std::shared_ptr<EsInfo>> ret;
    auto forbidden_zero_bit = getBits(1);
    if (forbidden_zero_bit != 0)
    {
        LOGD << "Syntax error";
        return std::list<std::shared_ptr<EsInfo>>();
    }
    skipBits(2);
    auto nal_unit_type = getBits(5);
    auto rete = std::make_shared<EsInfoH264>();
    rete->nalUnitType = nal_unit_type;
    switch (nal_unit_type)
    {
    case 0:
         rete->msg = "Unspecified";
         ret.push_back(rete);
        break;
    case 1:
        ret.push_back(slice_header(nal_unit_type));
        break;
    case 2:
         rete->msg = "Coded slice data partition A";
         ret.push_back(rete);
        break;
    case 3:
         rete->msg = "Coded slice data partition B";
         ret.push_back(rete);
        break;
    case 4:
        rete->msg = "Coded slice data partition C";
         ret.push_back(rete);
        break;
    case 5:
        ret.push_back(slice_header(nal_unit_type));
        break;
    case 6:
         rete->msg = "Supplemental enhancement information (SEI)";
         ret.push_back(rete);
        break;
    case 7:
        ret.push_back(seq_parameter_set_rbsp(nal_unit_type));
        break;
    case 8:
        ret.push_back(pic_parameter_set_rbsp(nal_unit_type));
        break;
    case 9:
         rete->msg = "Access unit delimiter";
         ret.push_back(rete);
        break;
    case 10:
         rete->msg = "End of sequence";
         ret.push_back(rete);
        break;
    case 11:
         rete->msg = "End of stream";
         ret.push_back(rete);
        break;
    case 12:
         rete->msg = "Filler data";
         ret.push_back(rete);
        break;
    default:
        if (nal_unit_type >= 13 && nal_unit_type <= 23)
        {
            rete->msg = "Reserved";
            ret.push_back(rete);
        }
        else
        {
            rete->msg = "Unspecified";
            ret.push_back(rete);
        }
    }

    return ret;
}

std::shared_ptr<EsInfoH264SliceHeader> H264EsParser::slice_header(int nal_unit_type)
{
    auto ret = std::make_shared<EsInfoH264SliceHeader>();
    ret->msg =  (nal_unit_type != 5) ? "Coded slice of a non-IDR picture" : "Coded slice of an ***IDR*** pictur";
    ret->nalUnitType = nal_unit_type;
    auto first_mb_in_slice = getBitsDecodeUGolomb();
    (void)first_mb_in_slice;
    auto slice_type = getBitsDecodeUGolomb();
    ret->sliceType = slice_type;
    switch (slice_type)
    {
    case 0:
        ret->sliceTypeStr = "slice_type: P";
        break;
    case 1:
        ret->sliceTypeStr = "slice_type: B";
        break;
    case 2:
        ret->sliceTypeStr = "slice_type: I";
        break;
    case 3:
        ret->sliceTypeStr = "slice_type: SP";
        break;
    case 4:
        ret->sliceTypeStr = "slice_type: SI";
        break;
    case 5:
        ret->sliceTypeStr = "slice_type: P";
        break;
    case 6:
        ret->sliceTypeStr = "slice_type: B";
        break;
    case 7:
        ret->sliceTypeStr = "slice_type: I";
        break;
    case 8:
        ret->sliceTypeStr = "slice_type: SP";
        break;
    case 9:
        ret->sliceTypeStr = "slice_type: SI";
        break;
    default:
        ret->sliceTypeStr = "slice_type: Uknown";
    }
    auto pic_parameter_set_id = getBitsDecodeUGolomb();
    ret->ppsId = pic_parameter_set_id;
    if (separate_colour_plane_flag == 1)
    {
        auto colour_plane_id = getBits(2);
        (void)colour_plane_id;
    }
    auto frame_num = getBits(log2_max_frame_num_minus4 + 4);
    (void)frame_num;
    if (!frame_mbs_only_flag)
    {
        auto field_pic_flag = getBits(1);
        ret->field = field_pic_flag;
        if (field_pic_flag)
        {
            auto bottom_field_flag = getBits(1);
            ret->top = !bottom_field_flag;
        }
    }

    return ret;
}

uint64_t H264EsParser::getBitsDecodeUGolomb()
{
    unsigned int leading_zeros = 0;
    while (getBits(1) == 0)
    {
        leading_zeros++;
        if (leading_zeros > 32)
            return 0; // safeguard
    }
    if (leading_zeros)
    {
        return (1 << leading_zeros) - 1 + getBits(leading_zeros);
    }
    else
    {
        return 0;
    }
}

void H264EsParser::scaling_list(uint8_t* scalingList, size_t sizeOfScalingList)
{
    uint8_t lastScale = 8;
    uint8_t nextScale = 8;
    for (auto j = 0u; j < sizeOfScalingList; j++)
    {
        if (nextScale != 0)
        {
            auto delta_scale = getBitsDecodeUGolomb();
            nextScale = (lastScale + delta_scale + 256) % 256;
            // useDefaultScalingMatrixFlag = ( j = = 0 && nextScale = = 0 )
        }
        scalingList[j] = (nextScale == 0) ? lastScale : nextScale;
        lastScale = scalingList[j];
    }
}

std::shared_ptr<EsInfoH264SequenceParameterSet> H264EsParser::seq_parameter_set_rbsp(int nal_unit_type)
{
    auto ret = std::make_shared<EsInfoH264SequenceParameterSet>();
    ret->nalUnitType = nal_unit_type;
    ret->msg = "Sequence parameter set: ";
    auto profile_idc = getBits(8);
    ret->profileIdc = profile_idc;
    std::ostringstream msg;
    msg << "profile: ";
    if (profile_idc == 66)
    {
        msg << "Baseline";
    }
    else if (profile_idc == 77)
    {
        msg << "Main";
    }
    else if (profile_idc == 88)
    {
        msg << "Extended";
    }
    else if (profile_idc == 100)
    {
        msg << "High";
    }
    else if (profile_idc == 110)
    {
        msg << "High 10";
    }
    else if (profile_idc == 122)
    {
        msg << "High 4:2:2";
    }
    else if (profile_idc == 244)
    {
        msg << "High 4:4:4";
    }
    else if (profile_idc == 44)
    {
        msg << "CAVLC 4:4:4";
    }
    else
    {
        msg << profile_idc;
    }
    auto constraint_set012_flag = getBits(3);
    (void)constraint_set012_flag;
    auto reserved_zero_5bits = getBits(5);
    (void)reserved_zero_5bits;
    auto level_idc = getBits(8);
    ret->levelIdc = level_idc;
    msg << " level: " << level_idc / 10 << "." << level_idc % 10;
    ret->msg += msg.str();
    auto seq_parameter_set_id = getBitsDecodeUGolomb();
    ret->spsId = seq_parameter_set_id;
    if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 244 ||
        profile_idc == 44 || profile_idc == 83 || profile_idc == 86 || profile_idc == 118 || profile_idc == 128 ||
        profile_idc == 138 || profile_idc == 139 || profile_idc == 134 || profile_idc == 135)
    {
        auto chroma_format_idc = getBitsDecodeUGolomb();
        if (chroma_format_idc == 3)
        {
            separate_colour_plane_flag = getBitsDecodeUGolomb();
        }
        auto bit_depth_luma_minus8 = getBitsDecodeUGolomb();
        auto bit_depth_chroma_minus8 = getBitsDecodeUGolomb();
        ret->lumaBits = bit_depth_luma_minus8 + 8;
        ret->chromaBits = bit_depth_chroma_minus8 + 8;
        auto qpprime_y_zero_transform_bypass_flag = getBits(1);
        (void)qpprime_y_zero_transform_bypass_flag;
        auto seq_scaling_matrix_present_flag = getBits(1);
        if (seq_scaling_matrix_present_flag)
        {
            for (auto i = 0; i < ((chroma_format_idc != 3) ? 8 : 12); i++)
            {
                auto seq_scaling_list_present_flag = getBits(1);
                if (seq_scaling_list_present_flag)
                {
                    if (i < 6)
                    {
                        uint8_t ScalingList4x4[16];
                        scaling_list(&ScalingList4x4[i], 16);
                    }
                    else
                    {
                        uint8_t ScalingList8x8[64];
                        scaling_list(&ScalingList8x8[i - 6], 64);
                    }
                }
            }
        }
    }
    log2_max_frame_num_minus4 = getBitsDecodeUGolomb();
    auto pic_order_cnt_type = getBitsDecodeUGolomb();
    if (pic_order_cnt_type == 0)
    {
        auto log2_max_pic_order_cnt_lsb_minus4 = getBitsDecodeUGolomb();
        (void)log2_max_pic_order_cnt_lsb_minus4;
    }
    else if (pic_order_cnt_type == 1)
    {
        skipBits(1);                                          // delta_pic_order_always_zero_flag
        auto offset_for_non_ref_pic = getBitsDecodeUGolomb(); // TODO: it is se(v) !
        (void)offset_for_non_ref_pic;
        auto offset_for_top_to_bottom_field = getBitsDecodeUGolomb(); // TODO: it is se(v) !
        (void)offset_for_top_to_bottom_field;
        auto num_ref_frames_in_pic_order_cnt_cycle = getBitsDecodeUGolomb();
        for (auto i = 0u; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
        {
            auto offset_for_ref_frame = getBitsDecodeUGolomb(); // TODO: it is se(v) !
            (void)offset_for_ref_frame;
        }
    }
    auto num_ref_frames = getBitsDecodeUGolomb();
    ret->numRefPics = num_ref_frames;
    skipBits(1); // gaps_in_frame_num_value_allowed_flag
    auto pic_width_in_mbs_minus1s = getBitsDecodeUGolomb();
    auto pic_height_in_map_units_minus1s = getBitsDecodeUGolomb();
    frame_mbs_only_flag = getBits(1); // 0 - coded field or coded frame; 1 - coded frame
    ret->width = (pic_width_in_mbs_minus1s + 1) * 16;
    ret->height = (2 - frame_mbs_only_flag) * (pic_height_in_map_units_minus1s + 1) * 16;

    return ret;
}

std::shared_ptr<EsInfoH264PictureParameterSet> H264EsParser::pic_parameter_set_rbsp(int nal_unit_type)
{
    auto ret = std::make_shared<EsInfoH264PictureParameterSet>();
    ret->msg = "Picture parameter set: ";
    ret->nalUnitType = nal_unit_type;
    auto pic_parameter_set_id = getBitsDecodeUGolomb();
    auto seq_parameter_set_id = getBitsDecodeUGolomb();
    ret->spsId = seq_parameter_set_id;
    ret->ppsId = pic_parameter_set_id;
    auto entropy_coding_mode_flag = getBitsDecodeUGolomb();
    if (entropy_coding_mode_flag == 0)
    {
        ret->msg += "entropy: CAVLC";
    }
    else
    {
        ret->msg += "entropy: CABAC";
    }

    return ret;
}
