#include <public/mpeg2ts.h>

namespace mpeg2ts
{

std::ostream& operator<<(std::ostream& ss, const PesPacket& rhs)
{
    ss << "-------------PesPacket------------- " << std::endl;
    ss << "packet_start_code_prefix:" << std::hex << rhs.packet_start_code_prefix << std::dec << std::endl;
    ss << "stream_id: " << (int)rhs.stream_id << std::endl;
    ss << "PES_packet_length: " << (int)rhs.PES_packet_length << std::endl;

    ss << "PES_scrambling_control: " << (int)rhs.PES_scrambling_control << std::endl;
    ss << "PES_priority: " << (int)rhs.PES_priority << std::endl;
    ss << "data_alignment_indicator: " << (int)rhs.data_alignment_indicator << std::endl;
    ss << "copyright: " << (int)rhs.copyright << std::endl;
    ss << "original_or_copy: " << (int)rhs.original_or_copy << std::endl;
    ss << "PTS_DTS_flags: " << (int)rhs.PTS_DTS_flags << std::endl;
    ss << "ESCR_flag: " << (int)rhs.ESCR_flag << std::endl;
    ss << "ES_rate_flag: " << (int)rhs.ES_rate_flag << std::endl;
    ss << "DSM_trick_mode_flag: " << (int)rhs.DSM_trick_mode_flag << std::endl;
    ss << "additional_copy_info_flag: " << (int)rhs.additional_copy_info_flag << std::endl;
    ss << "PES_CRC_flag: " << (int)rhs.PES_CRC_flag << std::endl;
    ss << "PES_extension_flag: " << (int)rhs.PES_extension_flag << std::endl;

    ss << "pts: " << (int)rhs.pts << std::endl;
    ss << "dts: " << (int)rhs.dts << std::endl;

    ss << "PES_header_data_length: " << (int)rhs.PES_header_data_length << std::endl;
    return ss;
}

}