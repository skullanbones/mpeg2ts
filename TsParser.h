/**
 * @class TsParser
 *
 * @brief Parse a transport stream packet
 *
 * @author skullanbones
 *
 * @version $Revision: 0.1 $
 *
 * @date $Date: 2018/02/17 14:16:20 $
 *
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */
#pragma once

#include <stdint.h>
#include "TsPacketInfo.h"
#include "GetBits.h"

const int TS_PACKET_SYNC_BYTE = 0x47;
const int TS_PACKET_SIZE = 188;
const int TS_PACKET_HEADER_SIZE = 4;
const int TS_PACKET_MAX_PAYLOAD_SIZE = (TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE);
const int TS_PACKET_ADAPTATION_FIELD_SIZE = 2;
const int TS_PACKET_PID_PAT = 0x0;  // PAT packet
const int TS_PACKET_PID_NULL = 0x1fff; // Null Packet


const int PCR_SIZE = 48 / 8;
const int OPCR_SIZE = 48 / 8;


/*!
 * transport stream header, see ISO/IEC 13818-1:2015
 * 4 bytes
 */
struct TsHeader
{
    uint8_t sync_byte;
    bool transport_error_indicator;
    bool payload_unit_start_indicator;
    bool transport_priority;
    uint16_t PID;
    uint8_t transport_scrambling_control;
    uint8_t adaptation_field_control;
    uint8_t continuity_counter;

    friend std::ostream& operator << (std::ostream& os, const TsHeader& rhs);

    std::string toString() const
    {
        std::ostringstream ss;
        ss << "-------------TsHeader------------- " << std::endl;
        ss << "sync_byte:  0x" << std::hex << (int)sync_byte << std::dec << std::endl;
        ss << "transport_error_indicator: " << (int)transport_error_indicator << std::endl;
        ss << "payload_unit_start_indicator: " << (int)payload_unit_start_indicator << std::endl;
        ss << "transport_priority: " << (int)transport_priority << std::endl;
        ss << "PID: " << PID << std::endl;
        ss << "transport_scrambling_control: " << (int)transport_scrambling_control << std::endl;
        ss << "adaptation_field_control: " << (int)adaptation_field_control << std::endl;
        ss << "continuity_counter: " << (int)continuity_counter << std::endl;
        return ss.str();
    }
};


/*!
 * adaptation field header, see ISO/IEC 13818-1:2015
 * 2 bytes
 */
struct TsAdaptationFieldHeader
{
    uint8_t adaptation_field_length;

    uint8_t adaptation_field_extension_flag : 1;
    uint8_t transport_private_data_flag     : 1;
    uint8_t splicing_point_flag             : 1;
    uint8_t OPCR_flag                       : 1;
    uint8_t PCR_flag                        : 1;
    uint8_t elementary_stream_priority_indicator : 1;
    uint8_t random_access_indicator         : 1;
    uint8_t discontinuity_indicator         : 1;
};

/*!
 * Adaptation field control values, see table 2-5,
 * in ISO/IEC 13818-1:2015
 */
enum TsAdaptationFieldControl {
    ts_adaptation_field_control_reserved        = 0x00,
    ts_adaptation_field_control_payload_only    = 0x01,
    ts_adaptation_field_control_adaptation_only = 0x02,
    ts_adaptation_field_control_adaptation_payload = 0x03
};


struct TsAdaptationFieldExtensionHeader {
    uint8_t adaptation_field_extension_length;

    uint8_t reserved                       : 5;
    uint8_t seamless_splice_flag           : 1;
    uint8_t piecewise_rate_flag            : 1;
    uint8_t ltw_flag                       : 1;
};


class TsParser : GetBits {
public:

    /*!
    * Parse information about one ts-packet to find useful information
    * like for example PES-start, PCR, DTS,
    * @param pkt Pointer to ts-packet.
    * @param info Ts-packet information describing this packet.
    */
    void parseTsPacketInfo(const uint8_t* packet, TsPacketInfo& outInfo);

    /*!
     * Checks if the ts-packet has correct sync byte.
     * @param packet Pointer to Ts-packet first byte.
     * @return True if valid sync byte, else false
     */
    bool checkSyncByte(const uint8_t* byte);

    /*!
     * Return a copy of the TsHeader.
     * @param packet Pointer to ts-packet.
     * @return TsHeader.
     */
    TsHeader parseTsHeader(const uint8_t* packet);

    /*!
    * Checks if a pts-packet has the adaptation field.
    * @param packet Pointer to ts-packet.
    * @return True if it has adaptation field, else false.
    */
    bool checkHasAdaptationField(TsHeader header);

    /*!
     * Return the adaptation field length.
     * @param packet Pointer to ts-packet.
     * @return The adaptation field length.
     */
     uint8_t parseAdaptationFieldLength(TsHeader hdr, const uint8_t* packet);

    /*!
     * Return a copy of the TsAdaptationFieldHeader
     * @param packet Pointer to ts-packet.
     * @return TsAdaptationFieldHeader
     */
    TsAdaptationFieldHeader parseAdaptationFieldHeader(const uint8_t* packet);

    /*!
     * Parse adaptation field data acoording to ISO/IEC 13818-1:2015
     * @param packet Pointer to ts-packet.
     * @param outInfo Return TsPacketInfo
     */
    void parseAdaptationFieldData(const uint8_t* packet, TsPacketInfo& outInfo);

    /*!
     * Parse PCR from a ts-packet
     * @param ptr Pointer to start of PCR adaptation field.
     * @return The PCR value.
     */
    uint64_t parsePcr(const uint8_t* buffer);

private:
    uint64_t mPacketErrorCounter; // Wrong sync byte
    uint64_t mPacketDiscontinuityErrorCounter; // Wrong continuity
};
