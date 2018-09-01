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

#pragma once

#include <iostream>
#include <map>

#define ENUM_TO_STR(ENUM) std::string(#ENUM)

// TS Packet
const int TS_PACKET_SYNC_BYTE = 0x47; // Section 2.4.3.3
const int TS_PACKET_SIZE = 188;       // Section 2.4.3
const int TS_PACKET_HEADER_SIZE = 4;  // Table 2-2
const int TS_PACKET_MAX_PAYLOAD_SIZE = (TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE);
const int TS_PACKET_ADAPTATION_FIELD_SIZE = 2;
const int TS_PACKET_PID_PAT = 0x00;                // PAT packet, Table 2-28
const int TS_PACKET_PID_CAT = 0x01;                // CAT packet, Table 2-28
const int TS_PACKET_PID_TDT = 0x02;                // TDT packet, Table 2-28
const int TS_PACKET_PID_IPMP = 0x03;               // IPMP packet, Table 2-28
const int TS_PACKET_PID_NULL = 0x1fff;             // Null Packet, Section 2.4.1
const int PES_PACKET_START_CODE_PREFIX = 0x000001; // Section 2.4.3.7


const int PCR_SIZE = 48 / 8;
const int OPCR_SIZE = 48 / 8;

const int PAT_PACKET_OFFSET_LENGTH = 5;
const int CRC32_SIZE = 4;
const int PAT_PACKET_PROGRAM_SIZE = 4;

const int PMT_PACKET_OFFSET_LENGTH = 9;
const int PMT_STREAM_TYPE_LENGTH = 5;

/*!
 * transport stream header
 * 4 bytes, see Table 2-2 – Transport packet of this Recommendation.
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
 * 2 bytes, see table 2-6 Transport stream adaptation field
 */
struct TsAdaptationFieldHeader
{
    uint8_t adaptation_field_length;

    bool discontinuity_indicator;
    bool random_access_indicator;
    bool elementary_stream_priority_indicator;
    bool PCR_flag;
    bool OPCR_flag;
    bool splicing_point_flag;
    bool transport_private_data_flag;
    bool adaptation_field_extension_flag;
};

/*!
 * Adaptation field control values, see table 2-5
 */
enum TsAdaptationFieldControl
{
    TS_ADAPATION_FIELD_CONTROL_RESERVED = 0x00,
    TS_ADAPTATION_FIELD_CONTROL_PAYLOAD_ONLY = 0x01,
    TS_ADAPTATION_FIELD_CONTROL_ADAPTATION_ONLY = 0x02,
    TS_ADAPTATION_FIELD_CONTROL_ADAPTATION_PAYLOAD = 0x03
};


struct TsAdaptationFieldExtensionHeader
{
    uint8_t adaptation_field_extension_length;

    uint8_t reserved : 5;
    uint8_t seamless_splice_flag : 1;
    uint8_t piecewise_rate_flag : 1;
    uint8_t ltw_flag : 1;
};

enum class ProgramType
{
    NIT,
    PMT,
    UserDefined
};

/*! @brief Program streams
 *
 * Table 2-30 – Program association section *
 *
 */
struct Program
{
    uint16_t program_number;

    union {
        uint16_t network_PID; // only for program_number=0
        uint16_t program_map_PID;
    };

    ProgramType type; // 2.4.4.5 Semantics

    /// @brief Comparison operator for comparing 2 PatTables
    bool operator==(const Program& rhs) const
    {
        return program_number == rhs.program_number && program_map_PID == rhs.program_map_PID;
    }

    bool operator!=(const Program& rhs) const
    {
        return !operator==(rhs);
    }
};


/*! @brief Table_id assignment values
 *
 * Table 2-31 - table_id assignment values
 *
 */
enum PsiTableId
{
    PSI_TABLE_ID_PAT = 0x00,        /*! Program Association Table Id */
    PSI_TABLE_ID_CAT = 0x01,        /*! Conditional Access Section Table Id */
    PSI_TABLE_ID_PMT = 0x02,        /*! Program Map Table Id */
    PSI_TABLE_ID_TDT = 0x03,        /*! Transport stream Desciption Table id */
    PSI_TABLE_ID_SDT = 0x04,        /*! ISO_IEC_14496_scene_description_section */
    PSI_TABLE_ID_ODT = 0x05,        /*! ISO_IEC_14496_object_descriptor_section */
    PSI_TABLE_ID_METADATA = 0x06,   /*! Metadata_section */
    PSI_TABLE_ID_IPMP = 0x07,       /*! IPMP Control Information Section */
    PSI_TABLE_ID_14496 = 0x08,      /*! ISO_IEC_14496_section */
    PSI_TABLE_ID_INCOMPLETE = 0xfe, /*! User defined */
    PSI_TABLE_ID_FORBIDDEN = 0xff   /*! Forbidden */
};

static std::map<PsiTableId, std::string> PsiTableToString =
{ { PSI_TABLE_ID_PAT, "PSI_TABLE_ID_PAT" },
  { PSI_TABLE_ID_CAT, "PSI_TABLE_ID_CAT" },
  { PSI_TABLE_ID_PMT, "PSI_TABLE_ID_PMT" },
  { PSI_TABLE_ID_TDT, "PSI_TABLE_ID_TDT" },
  { PSI_TABLE_ID_SDT, "PSI_TABLE_ID_SDT" },
  { PSI_TABLE_ID_ODT, "PSI_TABLE_ID_ODT" },
  { PSI_TABLE_ID_METADATA, "PSI_TABLE_ID_METADATA" },
  { PSI_TABLE_ID_IPMP, "PSI_TABLE_ID_IPMP" },
  { PSI_TABLE_ID_14496, "PSI_TABLE_ID_14496" },
  { PSI_TABLE_ID_INCOMPLETE, "PSI_TABLE_ID_INCOMPLETE" },
  { PSI_TABLE_ID_FORBIDDEN, "PSI_TABLE_ID_FORBIDDEN" } };

/*! @brief Stream type
 *
 * Table 2-34 - Stream type assignments.
 *
 */
enum StreamType
{
    STREAMTYPE_RESERVED = 0x00,
    STREAMTYPE_VIDEO_MPEG1 = 0X01,
    STREAMTYPE_VIDEO_MPEG2 = 0x02,
    STREAMTYPE_AUDIO_MPEG1 = 0X03,
    STREAMTYPE_AUDIO_MPEG2 = 0x04,
    STREAMTYPE_PRIVATE_TABLE = 0x05,
    STREAMTYPE_PRIVATE_PES = 0X06,
    STREAMTYPE_MHEG = 0X07,
    STREAMTYPE_VIDEO_MPEG4 = 0X10,
    STREAMTYPE_AUDIO_ADTS = 0x0F,
    STREAMTYPE_VIDEO_H264 = 0x1B,
    STREAMTYPE_VIDEO_H265 = 0X24,
    STREAMTYPE_AUDIO_AC3 = 0X81,
    STREAMTYPE_AUDIO_DTS = 0X82,
    STREAMTYPE_AUDIO_DOLBY_TRUE_HD = 0X83,
    STREAMTYPE_AUDIO_AC3_PLUS = 0X84,
    STREAMTYPE_AUDIO_EAC3 = 0X87,
    STREAMTYPE_Any = 0xFF // User private
};

static std::map<int, std::string> StreamTypeToString =
{ { STREAMTYPE_RESERVED, "STREAMTYPE_RESERVED" },
  { STREAMTYPE_VIDEO_MPEG1, "STREAMTYPE_VIDEO_MPEG1" },
  { STREAMTYPE_VIDEO_MPEG2, "STREAMTYPE_VIDEO_MPEG2" },
  { STREAMTYPE_AUDIO_MPEG1, "STREAMTYPE_AUDIO_MPEG1" },
  { STREAMTYPE_AUDIO_MPEG2, "STREAMTYPE_AUDIO_MPEG2" },
  { STREAMTYPE_PRIVATE_TABLE, "STREAMTYPE_PRIVATE_TABLE" },
  { STREAMTYPE_PRIVATE_PES, "STREAMTYPE_PRIVATE_PES" },
  { STREAMTYPE_MHEG, "STREAMTYPE_MHEG" },
  { STREAMTYPE_VIDEO_MPEG4, "STREAMTYPE_VIDEO_MPEG4" },
  { STREAMTYPE_AUDIO_ADTS, "STREAMTYPE_AUDIO_ADTS" },
  { STREAMTYPE_VIDEO_H264, "STREAMTYPE_VIDEO_H264" },
  { STREAMTYPE_VIDEO_H265, "STREAMTYPE_VIDEO_H265" },
  { STREAMTYPE_AUDIO_AC3, "STREAMTYPE_AUDIO_AC3" },
  { STREAMTYPE_AUDIO_DTS, "STREAMTYPE_AUDIO_DTS" },
  { STREAMTYPE_AUDIO_DOLBY_TRUE_HD, "STREAMTYPE_AUDIO_DOLBY_TRUE_HD" },
  { STREAMTYPE_AUDIO_AC3_PLUS, "STREAMTYPE_AUDIO_AC3_PLUS" },
  { STREAMTYPE_AUDIO_EAC3, "STREAMTYPE_AUDIO_EAC3" },
  { STREAMTYPE_Any, "STREAMTYPE_Any" } };


/*! @brief Stream_id assignments
 *
 *[ISO 13818-1] Table 2-22 – Stream_id assignments
 *
 */
enum StreamId
{
    STREAM_ID_program_stream_map = 0xBC,
    STREAM_ID_private_stream_1 = 0xBD,
    STREAM_ID_padding_stream = 0xBE,
    STREAM_ID_private_stream_2 = 0xBF,
    STREAM_ID_pes_audio_stream = 0xC0,
    STREAM_ID_pes_video_stream = 0xE0,
    STREAM_ID_ECM_stream = 0xF0,
    STREAM_ID_EMM_stream = 0xF1,
    STREAM_ID_DSMCC_stream = 0xF2,
    STREAM_ID_ITU_T_Rec_H222_1_type_E_stream = 0xF8,
    STREAM_ID_program_stream_directory = 0xFF
};
