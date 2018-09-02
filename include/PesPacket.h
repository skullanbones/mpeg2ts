#pragma once

#include <ostream>

/// Project files
#include "CommonTypes.h"

/*!
 * @class PES-Packet prototype containing buffer
 *
 */
struct PesPacket
{
    uint32_t packet_start_code_prefix;
    uint8_t stream_id;
    uint16_t PES_packet_length;

    // Extended packet
    // TODO move out by inheritance?
    bool PES_scrambling_control;
    bool PES_priority;
    bool data_alignment_indicator;
    bool copyright;
    bool original_or_copy;
    uint8_t PTS_DTS_flags;
    bool ESCR_flag;
    bool ES_rate_flag;
    bool DSM_trick_mode_flag;
    bool additional_copy_info_flag;
    bool PES_CRC_flag;
    bool PES_extension_flag;

    uint8_t PES_header_data_length;
    uint16_t elementary_data_offset; // where ES data begins

    int64_t pts;
    int64_t dts;

    ByteVector mPesBuffer;

    friend std::ostream& operator<<(std::ostream& ss, const PesPacket& rhs);
};
