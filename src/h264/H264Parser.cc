///
#include <sstream>
/// 3rd-party
#include <plog/Log.h>

/// Project files
#include "h264/H264Parser.h"

bool H264EsParser::analyze()
{
            resetBits(mPicture.data(), mPicture.size());
            
            auto forbidden_zero_bit = getBits(1);
            if (forbidden_zero_bit !=0)
            {
                LOGD << "Syntax error";
                return false;
            }
            skipBits(2);
            auto nal_unit_type = getBits(5);
            switch (nal_unit_type)
            {
            case 0: LOGD << "Unspecified";
                break;
            case 1: LOGD << "Coded slice of a non-IDR picture";
                slice_header(false);
                break;
            case 2: LOGD << "Coded slice data partition A";
                break;
            case 3: LOGD << "Coded slice data partition B";
                break;
            case 4: LOGD << "Coded slice data partition C";
                break;
            case 5: LOGD << "Coded slice of an ***IDR*** pictur";
                slice_header(true);
                break;
            case 6: LOGD << "Supplemental enhancement information (SEI)";
                break;
            case 7: LOGD << "Sequence parameter set";
                seq_parameter_set_rbsp();
                break;
            case 8: LOGD << "Picture parameter set";
                pic_parameter_set_rbsp();
                break;
            case 9: LOGD << "Access unit delimiter";
                break;
            case 10: LOGD << "End of sequence";
                break;
            case 11: LOGD << "End of stream";
                break;
            case 12: LOGD << "Filler data";
                break;
                default:
                    if (nal_unit_type >= 13 && nal_unit_type <=23)
                    {
                        LOGD << "Reserved";
                    }else
                    {
                        LOGD << "Unspecified";
                    }
            }

            return true;
}

void H264EsParser::slice_header(bool IdrPicFlag)
{
    auto first_mb_in_slice = getBitsDecodeUGolomb();
    (void)first_mb_in_slice;
    auto slice_type = getBitsDecodeUGolomb();
    switch (slice_type)
    {
        case 0: LOGD << "slice_type: P";break;
        case 1: LOGD << "slice_type: B";break;
        case 2: LOGD << "slice_type: I";break;
        case 3: LOGD << "slice_type: SP";break;
        case 4: LOGD << "slice_type: SI";break;
        case 5: LOGD << "slice_type: P";break;
        case 6: LOGD << "slice_type: B";break;
        case 7: LOGD << "slice_type: I";break;
        case 8: LOGD << "slice_type: SP";break;
        case 9: LOGD << "slice_type: SI";break;
        default:
            LOGD << "slice_type: Uknown";
    }
    auto pic_parameter_set_id = getBitsDecodeUGolomb();
    LOGD << "pic_parameter_set_id: " << pic_parameter_set_id;
    if( separate_colour_plane_flag == 1 )
    {
        auto colour_plane_id = getBits(2);
        (void)colour_plane_id;
    }
    auto frame_num = getBits(log2_max_frame_num_minus4 + 4);
    (void)frame_num;
    if( !frame_mbs_only_flag )
    {
        auto field_pic_flag = getBits(1);
        LOGD << (field_pic_flag ? "field " : "frame");
        if( field_pic_flag )
        {
            auto bottom_field_flag = getBits(1);
            LOGD << (bottom_field_flag ? "bottom" : "top");
        }
    }
}

uint64_t H264EsParser::getBitsDecodeUGolomb()
{
    unsigned int leading_zeros = 0;
    while (getBits(1) == 0) {
        leading_zeros++;
        if (leading_zeros > 32) return 0; // safeguard
    }
    if (leading_zeros) {
        return (1<<leading_zeros)-1+getBits(leading_zeros);
    } else {
        return 0;
    }
}

void H264EsParser::scaling_list(uint8_t* scalingList, size_t sizeOfScalingList)
{
    uint8_t lastScale = 8;
    uint8_t nextScale = 8;
    for(auto j = 0u; j < sizeOfScalingList; j++ )
    {
        if( nextScale != 0 )
        {
            auto delta_scale = getBitsDecodeUGolomb();
            nextScale = ( lastScale + delta_scale + 256 ) % 256;
            //useDefaultScalingMatrixFlag = ( j = = 0 && nextScale = = 0 )
        }
        scalingList[ j ] = ( nextScale == 0 ) ? lastScale : nextScale;
        lastScale = scalingList[ j ];
    }
}

void H264EsParser::seq_parameter_set_rbsp()
{
    auto profile_idc = getBits(8);
    std::ostringstream msg;
    msg << "profile: ";
    if (profile_idc == 66){
        msg << "Baseline";
    }else if (profile_idc == 77){
        msg << "Main";
    }else if (profile_idc == 88){
        msg << "Extended";
    }else if (profile_idc == 100){
        msg << "High";
    }else if (profile_idc == 110){
        msg << "High 10";
    }else if (profile_idc == 122){
        msg << "High 4:2:2";
    }else if (profile_idc == 244){
        msg << "High 4:4:4";
    }else if (profile_idc == 44){
        msg << "CAVLC 4:4:4";
    }else
    {
        msg << profile_idc;
    }
    auto constraint_set012_flag = getBits(3);
    (void)constraint_set012_flag;
    auto reserved_zero_5bits = getBits(5);
    (void)reserved_zero_5bits;
    auto level_idc = getBits(8);
    LOGD << msg.str() << " level: " << level_idc / 10 << "." << level_idc % 10;
    auto seq_parameter_set_id = getBitsDecodeUGolomb();
    LOGD << "seq_parameter_set_id: " << seq_parameter_set_id;
    if( profile_idc == 100 || profile_idc == 110 ||
        profile_idc == 122 || profile_idc == 244 || profile_idc == 44 ||
        profile_idc == 83 || profile_idc == 86 || profile_idc == 118 ||
        profile_idc == 128 || profile_idc == 138 || profile_idc == 139 ||
        profile_idc == 134 || profile_idc == 135 )
    {
        auto chroma_format_idc = getBitsDecodeUGolomb();
        if( chroma_format_idc == 3 )
        {
            separate_colour_plane_flag = getBitsDecodeUGolomb();
        }
        auto bit_depth_luma_minus8 = getBitsDecodeUGolomb();
        auto bit_depth_chroma_minus8 = getBitsDecodeUGolomb();
        LOGD << "bit depth luma/chroma: " << bit_depth_luma_minus8 + 8 << "/" << bit_depth_chroma_minus8 + 8;
        auto qpprime_y_zero_transform_bypass_flag = getBits(1);
        (void) qpprime_y_zero_transform_bypass_flag;
        auto seq_scaling_matrix_present_flag = getBits(1);
        if( seq_scaling_matrix_present_flag )
        {
            for(auto i = 0; i < ( ( chroma_format_idc != 3 ) ? 8 : 12 ); i++ )
            {
                auto seq_scaling_list_present_flag = getBits(1);
                if( seq_scaling_list_present_flag)
                {
                    if( i < 6 )
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
        (void) log2_max_pic_order_cnt_lsb_minus4;
    }
    else if (pic_order_cnt_type == 1)
    {
        skipBits(1);//delta_pic_order_always_zero_flag
        auto offset_for_non_ref_pic = getBitsDecodeUGolomb();//TODO: it is se(v) !
        (void) offset_for_non_ref_pic;
        auto offset_for_top_to_bottom_field = getBitsDecodeUGolomb();//TODO: it is se(v) !
        (void)offset_for_top_to_bottom_field;
        auto num_ref_frames_in_pic_order_cnt_cycle = getBitsDecodeUGolomb();
        for(auto i = 0u; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
        {
            auto offset_for_ref_frame = getBitsDecodeUGolomb();//TODO: it is se(v) !
            (void)offset_for_ref_frame;
        }
    }
    auto num_ref_frames = getBitsDecodeUGolomb();
    LOGD << "num_ref_frames: " << num_ref_frames;
    skipBits(1);//gaps_in_frame_num_value_allowed_flag
    auto pic_width_in_mbs_minus1s = getBitsDecodeUGolomb();
    auto pic_height_in_map_units_minus1s = getBitsDecodeUGolomb();
    frame_mbs_only_flag = getBits(1);// 0 - coded field or coded frame; 1 - coded frame
    LOGD << "width: " << (pic_width_in_mbs_minus1s + 1) * 16
         << " height: " << (2 - frame_mbs_only_flag) * (pic_height_in_map_units_minus1s + 1) * 16;
}

void H264EsParser::pic_parameter_set_rbsp()
{
    auto pic_parameter_set_id = getBitsDecodeUGolomb();
    auto seq_parameter_set_id = getBitsDecodeUGolomb();
    LOGD << "pic_parameter_set_id: " << pic_parameter_set_id << " seq_parameter_set_id:" << seq_parameter_set_id;
    auto entropy_coding_mode_flag = getBitsDecodeUGolomb();
    if (entropy_coding_mode_flag == 0)
    {
        LOGD << "entropy: CAVLC";
    }else
    {
        LOGD << "entropy: CABAC";
    }
}
