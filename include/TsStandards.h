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

const int PAT_PACKET_OFFSET_LENGTH = 5;
const int CRC32_SIZE = 4;
const int PAT_PACKET_PROGRAM_SIZE = 4;

const int PMT_PACKET_OFFSET_LENGTH = 9;
const int PMT_STREAM_TYPE_LENGTH = 5;


/*!
 * References in this file are taken from ISO/IEC 13818-1:2015
 * Fifth edition 2015-07-01
 * Information technology — Generic
 * coding of moving pictures and
 * associated audio information —
 * Part 1: Systems
 *
 * All references here after reference to this document.
 */

/*!
 * transport stream header
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

    friend std::ostream& operator<<(std::ostream& ss, const TsHeader& rhs)
    {
        ss << "-------------TsHeader------------- " << std::endl;
        ss << "sync_byte:  0x" << std::hex << (int)rhs.sync_byte << std::dec << std::endl;
        ss << "transport_error_indicator: " << (int)rhs.transport_error_indicator << std::endl;
        ss << "payload_unit_start_indicator: " << (int)rhs.payload_unit_start_indicator << std::endl;
        ss << "transport_priority: " << (int)rhs.transport_priority << std::endl;
        ss << "PID: " << rhs.PID << std::endl;
        ss << "transport_scrambling_control: " << (int)rhs.transport_scrambling_control << std::endl;
        ss << "adaptation_field_control: " << (int)rhs.adaptation_field_control << std::endl;
        ss << "continuity_counter: " << (int)rhs.continuity_counter << std::endl;
        return ss;
    }
};


/*!
 * adaptation field header
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
 * Adaptation field control values, see table 2-5
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
    virtual ~PsiTable() = default;

    uint8_t table_id;
    bool section_syntax_indicator;
    uint16_t section_length;
    uint16_t transport_stream_id;
    uint8_t version_number;
    bool current_next_indicator;
    uint8_t section_number;
    uint8_t last_section_number;

    friend std::ostream& operator<<(std::ostream& ss, const PsiTable& rhs)
    {
        ss << "-------------PsiTable------------- " << std::endl;
        ss << "table_id:" << std::hex << (int)rhs.table_id << std::dec << std::endl;
        ss << "section_syntax_indicator: " << (int)rhs.section_syntax_indicator << std::endl;
        ss << "section_length: " << (int)rhs.section_length << std::endl;
        ss << "transport_stream_id: " << (int)rhs.transport_stream_id << std::endl;
        ss << "version_number: " << rhs.version_number << std::endl;
        ss << "current_next_indicator: " << (int)rhs.current_next_indicator << std::endl;
        ss << "section_number: " << (int)rhs.section_number << std::endl;
        ss << "last_section_number: " << (int)rhs.last_section_number << std::endl;
        return ss;
    }
};

class PatTable : public PsiTable
{
public:
    std::vector<Program> programs;
    uint32_t CRC_32;

    friend std::ostream& operator<<(std::ostream& ss, const PatTable& rhs)
    {
        ss << "-------------PatTable------------- " << std::endl;
        // ss << static_cast<const PsiTable&>(rhs) << std::endl;
        // ss << PsiTable::operator<<(rhs) << std::endl;
        ss << "programs.size(): " << (int)rhs.programs.size() << std::endl;
        for (unsigned int i = 0; i < rhs.programs.size(); i++)
        {
            ss << "-------------program " << i << "--------------" << std::endl;
            ss << "program_number: " << rhs.programs[i].program_number << std::endl;
            ss << "program_map_PID: " << rhs.programs[i].program_map_PID << std::endl;
        }

        return ss;
    }
};

struct StreamTypeHeader
{
    uint8_t stream_type;
    uint16_t elementary_PID;
    uint16_t ES_info_length;
};

class PmtTable : public PsiTable
{
public:
    uint16_t PCR_PID;
    uint16_t program_info_length;
    //    std::vector<Descriptor> descriptors; // TODO
    std::vector<StreamTypeHeader> streams;


    friend std::ostream& operator<<(std::ostream& ss, const PmtTable& rhs)
    {
        ss << "-------------PmtTable------------- " << std::endl;
        ss << "PCR_PID: " << (int)rhs.PCR_PID << std::endl;
        ss << "program_info_length: " << (int)rhs.program_info_length << std::endl;
        ss << "streams.size(): " << (int)rhs.streams.size() << std::endl;
        for (unsigned int i = 0; i < rhs.streams.size(); i++)
        {
            ss << "-------------stream " << i << "--------------" << std::endl;
            ss << "stream_type: " << (int)rhs.streams[i].stream_type << std::endl;
            ss << "elementary_PID: " << (int)rhs.streams[i].elementary_PID << std::endl;
            ss << "ES_info_length: " << (int)rhs.streams[i].ES_info_length << std::endl;
        }

        return ss;
    }
};


/// @brief Parsed PES
// TODO: move to own file
class PesPacket
{
protected:
};

/*! @brief Table_id assignment values
 * Table 2-31 - table_id assignment values
 *
 */
enum PsiTableId_e
{
    PSI_TABLE_ID_PAT = 0x00, /*! Program Association Table Id */
    PSI_TABLE_ID_CAT = 0x01, /*! Conditional Access Section Table Id */
    PSI_TABLE_ID_PMT = 0x02, /*! Program Map Table Id */
    PSI_TABLE_ID_TDT = 0x03  /*! Optional Transport stream Desciption Table id */
};