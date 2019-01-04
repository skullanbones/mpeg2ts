
#include <iostream>
#include <sstream>

/// 3rd-party
#include <plog/Log.h>

/// Project files
#include "h264/H264Parser.h"

namespace h264
{


void H264EsParser::analyze()
{
    resetBits(mPicture.data(), mPicture.size());

    auto forbidden_zero_bit = getBits(1);
    if (forbidden_zero_bit != 0)
    {
        LOGD << "Syntax error: forbidden_zero_bit != 0";
    }
    skipBits(2);
    auto nal_unit_type = static_cast<int>(getBits(5));
    EsInfoH264 info;
    info.type = H264InfoType::Info;
    info.nalUnitType = nal_unit_type;
    switch (nal_unit_type)
    {
    case 0:
        info.msg = "Unspecified";
        m_infos.push_back(info);
        break;
    case 1:
        info.type = H264InfoType::SliceHeader;
        slice_header(nal_unit_type, info);
        m_infos.push_back(info);
        break;
    case 2:
        info.msg = "Coded slice data partition A";
        m_infos.push_back(info);
        break;
    case 3:
        info.msg = "Coded slice data partition B";
        m_infos.push_back(info);
        break;
    case 4:
        info.msg = "Coded slice data partition C";
        m_infos.push_back(info);
        break;
    case 5:
        info.type = H264InfoType::SliceHeader;
        slice_header(nal_unit_type, info);
        m_infos.push_back(info);
        break;
    case 6:
    {
        uint64_t payloadType = 0;
        while (getBits(8) == 0xff)
        {
            payloadType += 255;
        }
        auto last_payload_type_byte = getBits(8);
        payloadType += last_payload_type_byte;

        info.msg = "Supplemental enhancement information (SEI), type: " + seipayloadTypeToString(payloadType);
    }
        m_infos.push_back(info);
        break;
    case 7:
        info.type = H264InfoType::SequenceParameterSet;
        seq_parameter_set_rbsp(nal_unit_type, info);
        m_infos.push_back(info);
        break;
    case 8:
        info.type = H264InfoType::PictureParameterSet;
        pic_parameter_set_rbsp(nal_unit_type, info);
        m_infos.push_back(info);
        break;
    case 9:
        info.msg = "Access unit delimiter";
        m_infos.push_back(info);
        break;
    case 10:
        info.msg = "End of sequence";
        m_infos.push_back(info);
        break;
    case 11:
        info.msg = "End of stream";
        m_infos.push_back(info);
        break;
    case 12:
        info.msg = "Filler data";
        m_infos.push_back(info);
        break;
    default:
        if (nal_unit_type >= 13 && nal_unit_type <= 23)
        {
            info.msg = "Reserved";
            m_infos.push_back(info);
        }
        else
        {
            info.msg = "Unspecified";
            m_infos.push_back(info);
        }
    }
}


std::string H264EsParser::seipayloadTypeToString(uint64_t payloadType)
{
    if (payloadType == 0)
    {
        return "buffering_period(payloadSize)";
    }
    else if (payloadType == 1)
    {
        return "pic_timing(payloadSize)";
    }
    else if (payloadType == 2)
    {
        return "pan_scan_rect(payloadSize)";
    }
    else if (payloadType == 3)
    {
        return "filler_payload(payloadSize)";
    }
    else if (payloadType == 4)
    {
        return "user_data_registered_itu_t_t35(payloadSize)";
    }
    else if (payloadType == 5)
    {
        return "user_data_unregistered(payloadSize)";
    }
    else if (payloadType == 6)
    {
        return "recovery_point(payloadSize)";
    }
    else if (payloadType == 7)
    {
        return "dec_ref_pic_marking_repetition(payloadSize)";
    }
    else if (payloadType == 8)
    {
        return "spare_pic(payloadSize)";
    }
    else if (payloadType == 9)
    {
        return "scene_info(payloadSize)";
    }
    else if (payloadType == 10)
    {
        return "sub_seq_info(payloadSize)";
    }
    else if (payloadType == 11)
    {
        return "sub_seq_layer_characteristics(payloadSize)";
    }
    else if (payloadType == 12)
    {
        return "sub_seq_characteristics(payloadSize)";
    }
    else if (payloadType == 13)
    {
        return "full_frame_freeze(payloadSize)";
    }
    else if (payloadType == 14)
    {
        return "full_frame_freeze_release(payloadSize)";
    }
    else if (payloadType == 15)
    {
        return "full_frame_snapshot(payloadSize)";
    }
    else if (payloadType == 16)
    {
        return "progressive_refinement_segment_start(payloadSize)";
    }
    else if (payloadType == 17)
    {
        return "progressive_refinement_segment_end(payloadSize)";
    }
    else if (payloadType == 18)
    {
        return "motion_constrained_slice_group_set(payloadSize)";
    }
    return "reserved_sei_message(payloadSize)";
}


void H264EsParser::slice_header(int nal_unit_type, EsInfoH264& info)
{
    info.msg = (nal_unit_type != 5) ? "Coded slice of a non-IDR picture" : "Coded slice of an ***IDR*** picture";
    info.nalUnitType = nal_unit_type;
    auto first_mb_in_slice = getBitsDecodeUGolomb();
    (void)first_mb_in_slice;
    LOGD << "first_mb_in_slice " << first_mb_in_slice;
    auto slice_type = static_cast<int>(getBitsDecodeUGolomb());
    info.slice.sliceType = slice_type;
    switch (slice_type)
    {
    case 0:
        info.slice.sliceTypeStr = "slice_type: P";
        break;
    case 1:
        info.slice.sliceTypeStr = "slice_type: B";
        break;
    case 2:
        info.slice.sliceTypeStr = "slice_type: I";
        break;
    case 3:
        info.slice.sliceTypeStr = "slice_type: SP";
        break;
    case 4:
        info.slice.sliceTypeStr = "slice_type: SI";
        break;
    case 5:
        info.slice.sliceTypeStr = "slice_type: P";
        break;
    case 6:
        info.slice.sliceTypeStr = "slice_type: B";
        break;
    case 7:
        info.slice.sliceTypeStr = "slice_type: I";
        break;
    case 8:
        info.slice.sliceTypeStr = "slice_type: SP";
        break;
    case 9:
        info.slice.sliceTypeStr = "slice_type: SI";
        break;
    default:
        info.slice.sliceTypeStr = "slice_type: Uknown";
    }
    auto pic_parameter_set_id = getBitsDecodeUGolomb();
    info.slice.ppsId = static_cast<int>(pic_parameter_set_id);
    if (separate_colour_plane_flag == 1)
    {
        auto colour_plane_id = getBits(2);
        (void)colour_plane_id;
    }
    info.slice.frame_num = static_cast<int>(getBits(static_cast<uint8_t>(log2_max_frame_num_minus4 + 4)));
    if (!frame_mbs_only_flag) // field or frame
    {
        auto field_pic_flag = getBits(1);
        info.slice.field = field_pic_flag;
        if (field_pic_flag)
        {
            auto bottom_field_flag = getBits(1);
            info.slice.top = !bottom_field_flag;
        }
    }
    else
    {
        info.slice.field = 0; // only frames
    }
}


uint64_t H264EsParser::getBitsDecodeUGolomb()
{
    uint8_t leading_zeros = 0;
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


void H264EsParser::scaling_list(uint8_t* scalingList, std::size_t sizeOfScalingList)
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


void H264EsParser::seq_parameter_set_rbsp(int nal_unit_type, EsInfoH264& info)
{
    info.nalUnitType = nal_unit_type;
    info.msg = "Sequence parameter set: ";
    auto profile_idc = getBits(8);
    info.sps.profileIdc = static_cast<int>(profile_idc);
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
    // auto reserved_zero_5bits = getBits(5);
    //(void)reserved_zero_5bits;
    skipBits(1);
    LOGD << "constraint_set4_flag " << getBits(1);
    skipBits(3);
    auto level_idc = getBits(8);
    info.sps.levelIdc = static_cast<int>(level_idc);
    msg << " level: " << level_idc / 10 << "." << level_idc % 10;
    info.msg += msg.str();
    auto seq_parameter_set_id = getBitsDecodeUGolomb();
    info.sps.spsId = static_cast<int>(seq_parameter_set_id);
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
        info.sps.lumaBits = static_cast<int>(bit_depth_luma_minus8 + 8);
        info.sps.chromaBits = static_cast<int>(bit_depth_chroma_minus8 + 8);
        auto qpprime_y_zero_transform_bypass_flag = getBits(1);
        (void)qpprime_y_zero_transform_bypass_flag;
        auto seq_scaling_matrix_present_flag = getBits(1);
        if (seq_scaling_matrix_present_flag)
        {
            for (auto i = 0; i < ((chroma_format_idc != 3) ? 8 : 12); ++i)
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
    log2_max_frame_num_minus4 = static_cast<uint8_t>(getBitsDecodeUGolomb());
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
        for (auto i = 0u; i < num_ref_frames_in_pic_order_cnt_cycle; ++i)
        {
            auto offset_for_ref_frame = getBitsDecodeUGolomb(); // TODO: it is se(v) !
            (void)offset_for_ref_frame;
        }
    }
    auto num_ref_frames = getBitsDecodeUGolomb();
    info.sps.numRefPics = static_cast<int>(num_ref_frames);
    skipBits(1); // gaps_in_frame_num_value_allowed_flag
    auto pic_width_in_mbs_minus1s = getBitsDecodeUGolomb();
    auto pic_height_in_map_units_minus1s = getBitsDecodeUGolomb();
    frame_mbs_only_flag = getBits(1); // 0 - coded field or coded frame; 1 - coded frame
    info.sps.width = static_cast<int>((pic_width_in_mbs_minus1s + 1) * 16);
    info.sps.height = static_cast<int>((2 - frame_mbs_only_flag) * (pic_height_in_map_units_minus1s + 1) * 16);
    if (!frame_mbs_only_flag)
    {
        // coded pictures of the coded video sequence may either be coded fields or coded frames
        auto mb_adaptive_frame_field_flag = getBits(1);
        LOGD << (mb_adaptive_frame_field_flag ? "interlaced MBAFF" : "interlaced PAFF");
        // mb_adaptive_frame_field_flag is 1 -> possible mbaff (macroblocks fields and frames)
        // else no switching between frame and field macroblocks within a picture
    }
    else
    {
        LOGD << "progressive";
        // every coded picture of the coded video sequence is a coded frame containing only frame
        // macroblocks
    }

    auto direct_8x8_inference_flag = getBits(1);
    (void)direct_8x8_inference_flag;
    auto frame_cropping_flag = getBits(1);
    if (frame_cropping_flag)
    {
        auto frame_crop_left_offset = getBitsDecodeUGolomb();
        (void)frame_crop_left_offset;
        auto frame_crop_right_offset = getBitsDecodeUGolomb();
        (void)frame_crop_right_offset;
        auto frame_crop_top_offset = getBitsDecodeUGolomb();
        (void)frame_crop_top_offset;
        auto frame_crop_bottom_offset = getBitsDecodeUGolomb();
        (void)frame_crop_bottom_offset;
    }
    auto vui_parameters_present_flag = getBits(1);
    if (vui_parameters_present_flag)
    {
        parse_vui();
    }
}


void H264EsParser::parse_vui()
{
    auto aspect_ratio_info_present_flag = getBits(1);
    if (aspect_ratio_info_present_flag)
    {
        auto aspect_ratio_idc = getBits(8);
        LOGD << "aspect_ratio_info_present_flag: aspect_ratio_idc: " << aspect_ratio_idc;
        if (aspect_ratio_idc == 255)
        {
            skipBits(16);
            skipBits(16);
        }
    }
    auto overscan_info_present_flag = getBits(1);
    if (overscan_info_present_flag)
    {
        auto overscan_appropriate_flag = getBits(1);
        (void)overscan_appropriate_flag;
    }
    auto video_signal_type_present_flag = getBits(1);
    if (video_signal_type_present_flag)
    {
        auto video_format = getBits(3);
        (void)video_format;
        auto video_full_range_flag = getBits(1);
        (void)video_full_range_flag;
        auto colour_description_present_flag = getBits(1);
        LOGD << "video_signal_type_present_flag: video_format: " << video_format
             << ", video_full_range_flag:" << video_full_range_flag;
        if (colour_description_present_flag)
        {
            auto colour_primaries = getBits(8);
            (void)colour_primaries;
            auto transfer_characteristics = getBits(8);
            (void)transfer_characteristics;
            auto matrix_coefficients = getBits(8);
            (void)matrix_coefficients;
            LOGD << "colour_description_present_flag: colour_primaries: " << colour_primaries
                 << ", transfer_characteristics:" << transfer_characteristics
                 << ", matrix_coefficients:" << matrix_coefficients;
        }
    }
    auto chroma_loc_info_present_flag = getBits(1);
    if (chroma_loc_info_present_flag)
    {
        auto chroma_sample_loc_type_top_field = getBitsDecodeUGolomb();
        (void)chroma_sample_loc_type_top_field;
        auto chroma_sample_loc_type_bottom_field = getBitsDecodeUGolomb();
        (void)chroma_sample_loc_type_bottom_field;
        LOGD << "chroma_loc_info_present_flag: chroma_sample_loc_type_top_field: " << chroma_sample_loc_type_top_field
             << ", chroma_sample_loc_type_bottom_field:" << chroma_sample_loc_type_bottom_field;
    }
    auto timing_info_present_flag = getBits(1);
    if (timing_info_present_flag)
    {
        // TODO: There is a bug here. I can not find it. It seems everything up to here is fine but
        // we get wrong values in below code :(
        auto num_units_in_tick = getBits(32);
        (void)num_units_in_tick;
        auto time_scale = getBits(32);
        (void)time_scale;
        auto fixed_frame_rate_flag = getBits(1);
        (void)fixed_frame_rate_flag;
        LOGD << "timing_info_present_flag: num: " << num_units_in_tick << ", scale: " << time_scale
             << ", fixed frame rate: " << fixed_frame_rate_flag;
        /*for (int i =0; i<32;++i)
        {
            std::cout << getBits(1) << " ";
        }
        std::cout << '\n';
        for (int i =0; i<32;++i)
        {
            std::cout << getBits(1) << " ";
        }
        std::cout << '\n';
        for (int i =0; i<1;++i)
        {
            std::cout << getBits(1) << " ";
        }
        std::cout << '\n';*/
    }
    auto nal_hrd_parameters_present_flag = getBits(1);
    if (nal_hrd_parameters_present_flag)
    {
        LOGE << "Not supported can not parse any further";
        // hrd_parameters();
    }
    auto vcl_hrd_parameters_present_flag = getBits(1);
    if (vcl_hrd_parameters_present_flag)
    {
        LOGE << "Not supported can not parse any further";
        // hrd_parameters();
    }
    if (nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag)
    {
        auto low_delay_hrd_flag = getBits(1);
        (void)low_delay_hrd_flag;
    }
    auto pic_struct_present_flag = getBits(1);
    LOGD << "pic_struct_present_flag " << static_cast<bool>(pic_struct_present_flag);
    auto bitstream_restriction_flag = getBits(1);
    if (bitstream_restriction_flag)
    {
        LOGE << "bitstream_restriction_flag";
    }
}


void H264EsParser::pic_parameter_set_rbsp(int nal_unit_type, EsInfoH264& info)
{
    info.msg = "Picture parameter set: ";
    info.nalUnitType = nal_unit_type;
    auto pic_parameter_set_id = getBitsDecodeUGolomb();
    auto seq_parameter_set_id = getBitsDecodeUGolomb();
    info.pps.spsId = static_cast<int>(seq_parameter_set_id);
    info.pps.ppsId = static_cast<int>(pic_parameter_set_id);
    auto entropy_coding_mode_flag = getBitsDecodeUGolomb();
    if (entropy_coding_mode_flag == 0)
    {
        info.msg += "entropy: CAVLC";
    }
    else
    {
        info.msg += "entropy: CABAC";
    }
}

}