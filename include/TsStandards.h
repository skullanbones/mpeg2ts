//
// Created by microlab on 2/24/18.
//
#include <vector>

#pragma once


const int TS_PACKET_SYNC_BYTE = 0x47;
const int TS_PACKET_SIZE = 188;
const int TS_PACKET_HEADER_SIZE = 4;
const int TS_PACKET_MAX_PAYLOAD_SIZE = (TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE);
const int TS_PACKET_ADAPTATION_FIELD_SIZE = 2;
const int TS_PACKET_PID_PAT = 0x0;     // PAT packet
const int TS_PACKET_PID_NULL = 0x1fff; // Null Packet


const int PCR_SIZE = 48 / 8;
const int OPCR_SIZE = 48 / 8;

// TODO fix better naming
const int PAT_OFFSET_LENGTH = 5;
const int CRC32_SIZE = 4;
const int PAT_PROGRAM_SIZE = 4;


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

    friend std::ostream& operator<<(std::ostream& os, const TsHeader& rhs);

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
    uint8_t transport_private_data_flag : 1;
    uint8_t splicing_point_flag : 1;
    uint8_t OPCR_flag : 1;
    uint8_t PCR_flag : 1;
    uint8_t elementary_stream_priority_indicator : 1;
    uint8_t random_access_indicator : 1;
    uint8_t discontinuity_indicator : 1;
};

/*!
 * Adaptation field control values, see table 2-5,
 * in ISO/IEC 13818-1:2015
 */
enum TsAdaptationFieldControl
{
    ts_adaptation_field_control_reserved = 0x00,
    ts_adaptation_field_control_payload_only = 0x01,
    ts_adaptation_field_control_adaptation_only = 0x02,
    ts_adaptation_field_control_adaptation_payload = 0x03
};


struct TsAdaptationFieldExtensionHeader
{
    uint8_t adaptation_field_extension_length;

    uint8_t reserved : 5;
    uint8_t seamless_splice_flag : 1;
    uint8_t piecewise_rate_flag : 1;
    uint8_t ltw_flag : 1;
};

/// @brief Base class for all tables
/* Taken from ISO-IEC 13818-1 Table 2-30 – Program association section */
// TODO: move to own file
struct Program
{
    uint16_t program_number;
    // uint16_t network_PID; only for program_number=0
    uint16_t program_map_PID;
};


class PsiTable
{
    public:
    uint8_t table_id;
    bool section_syntax_indicator;
    uint16_t section_length;
    uint16_t transport_stream_id;
    uint8_t version_number;
    bool current_next_indicator;
    uint8_t section_number;
    uint8_t last_section_number;
    std::vector<Program> programs;
    // std::vector
    uint32_t CRC_32;
};

/// @brief Parsed PES
// TODO: move to own file
class PesPacket
{
    protected:
};

/*! @brief Table_id assignment values
 * [ISO 13818-1] Table 2-31 - table_id assignment values
 *
 */
enum PsiTableId_e
{
    PSI_TABLE_ID_PAT = 0x00, /*! Program Association Table Id */
    PSI_TABLE_ID_CAT = 0x01, /*! Conditional Access Section Table Id */
    PSI_TABLE_ID_PMT = 0x02, /*! Program Map Table Id */
    PSI_TABLE_ID_TDT = 0x03  /*! Optional Transport stream Desciption Table id */
};