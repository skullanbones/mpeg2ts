#include <public/mpeg2ts.h>

namespace mpeg2ts
{

std::ostream& operator<<(std::ostream& ss, const PesPacket& rhs)
{
    ss << "-------------PesPacket------------- " << std::endl;
    ss << "packet_start_code_prefix:" << std::hex << rhs.packet_start_code_prefix << std::dec << std::endl;
    ss << "stream_id: " << static_cast<int>(rhs.stream_id) << std::endl;
    ss << "PES_packet_length: " << static_cast<int>(rhs.PES_packet_length) << std::endl;

    ss << "PES_scrambling_control: " << static_cast<int>(rhs.PES_scrambling_control) << std::endl;
    ss << "PES_priority: " << static_cast<int>(rhs.PES_priority) << std::endl;
    ss << "data_alignment_indicator: " << static_cast<int>(rhs.data_alignment_indicator) << std::endl;
    ss << "copyright: " << static_cast<int>(rhs.copyright) << std::endl;
    ss << "original_or_copy: " << static_cast<int>(rhs.original_or_copy) << std::endl;
    ss << "PTS_DTS_flags: " << static_cast<int>(rhs.PTS_DTS_flags) << std::endl;
    ss << "ESCR_flag: " << static_cast<int>(rhs.ESCR_flag) << std::endl;
    ss << "ES_rate_flag: " << static_cast<int>(rhs.ES_rate_flag) << std::endl;
    ss << "DSM_trick_mode_flag: " << static_cast<int>(rhs.DSM_trick_mode_flag) << std::endl;
    ss << "additional_copy_info_flag: " << static_cast<int>(rhs.additional_copy_info_flag) << std::endl;
    ss << "PES_CRC_flag: " << static_cast<int>(rhs.PES_CRC_flag) << std::endl;
    ss << "PES_extension_flag: " << static_cast<int>(rhs.PES_extension_flag) << std::endl;

    ss << "pts: " << rhs.pts << std::endl;
    ss << "dts: " << rhs.dts << std::endl;

    ss << "PES_header_data_length: " << static_cast<int>(rhs.PES_header_data_length) << std::endl;
    return ss;
}
}
