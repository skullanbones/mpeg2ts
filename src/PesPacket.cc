
#include "mpeg2ts.h"

namespace mpeg2ts
{

std::ostream& operator<<(std::ostream& ss, const PesPacket& rhs)
{
    ss << "\n-------------PesPacket-------------\n";
    ss << "packet_start_code_prefix:" << std::hex << rhs.packet_start_code_prefix << std::dec << '\n';
    ss << "stream_id: " << static_cast<int>(rhs.stream_id) << '\n';
    ss << "PES_packet_length: " << static_cast<int>(rhs.PES_packet_length) << '\n';

    ss << "PES_scrambling_control: " << static_cast<int>(rhs.PES_scrambling_control) << '\n';
    ss << "PES_priority: " << static_cast<int>(rhs.PES_priority) << '\n';
    ss << "data_alignment_indicator: " << static_cast<int>(rhs.data_alignment_indicator) << '\n';
    ss << "copyright: " << static_cast<int>(rhs.copyright) << '\n';
    ss << "original_or_copy: " << static_cast<int>(rhs.original_or_copy) << '\n';
    ss << "PTS_DTS_flags: " << static_cast<int>(rhs.PTS_DTS_flags) << '\n';
    ss << "ESCR_flag: " << static_cast<int>(rhs.ESCR_flag) << '\n';
    ss << "ES_rate_flag: " << static_cast<int>(rhs.ES_rate_flag) << '\n';
    ss << "DSM_trick_mode_flag: " << static_cast<int>(rhs.DSM_trick_mode_flag) << '\n';
    ss << "additional_copy_info_flag: " << static_cast<int>(rhs.additional_copy_info_flag) << '\n';
    ss << "PES_CRC_flag: " << static_cast<int>(rhs.PES_CRC_flag) << '\n';
    ss << "PES_extension_flag: " << static_cast<int>(rhs.PES_extension_flag) << '\n';

    ss << "pts: " << rhs.pts << '\n';
    ss << "dts: " << rhs.dts << '\n';

    ss << "PES_header_data_length: " << static_cast<int>(rhs.PES_header_data_length) << '\n';
    return ss;
}
}
