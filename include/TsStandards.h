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
#pragma once

#include <vector>
#include <map>

// Project files
#include "CommonTypes.h"

#define ENUM_TO_STR(ENUM) std::string(#ENUM)

// TS Packet
const int TS_PACKET_SYNC_BYTE = 0x47;
const int TS_PACKET_SIZE = 188;
const int TS_PACKET_HEADER_SIZE = 4;
const int TS_PACKET_MAX_PAYLOAD_SIZE = (TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE);
const int TS_PACKET_ADAPTATION_FIELD_SIZE = 2;
const int TS_PACKET_PID_PAT = 0x00;     // PAT packet, Table 2-28
const int TS_PACKET_PID_CAT = 0x01;     // CAT packet, Table 2-28
const int TS_PACKET_PID_TDT = 0x02;     // TDT packet, Table 2-28
const int TS_PACKET_PID_IPMP = 0x03;    // IPMP packet, Table 2-28
const int TS_PACKET_PID_NULL = 0x1fff; // Null Packet
const int PES_PACKET_START_CODE_PREFIX = 0x000001; // Section 2.4.3.7


const int PCR_SIZE = 48 / 8;
const int OPCR_SIZE = 48 / 8;

const int PAT_PACKET_OFFSET_LENGTH = 5;
const int CRC32_SIZE = 4;
const int PAT_PACKET_PROGRAM_SIZE = 4;

const int PMT_PACKET_OFFSET_LENGTH = 9;
const int PMT_STREAM_TYPE_LENGTH = 5;

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
    uint16_t transport_stream_id; // TODO program_number for PMT? Use Union?
    uint8_t version_number;
    bool current_next_indicator;
    uint8_t section_number;
    uint8_t last_section_number;
    uint32_t CRC_32;

    friend std::ostream& operator<<(std::ostream& ss, const PsiTable& rhs)
    {
        ss << "-------------PsiTable------------- " << std::endl;
        ss << "table_id:" << std::hex << (int)rhs.table_id << std::dec << std::endl;
        ss << "section_syntax_indicator: " << (int)rhs.section_syntax_indicator << std::endl;
        ss << "section_length: " << (int)rhs.section_length << std::endl;
        ss << "transport_stream_id: " << (int)rhs.transport_stream_id << std::endl;
        ss << "version_number: " << (int)rhs.version_number << std::endl;
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

struct CatDescriptor
{
};

class CatTable : public PsiTable
{
public:
    std::vector<CatDescriptor> descriptors;
};


/// @brief Parsed PES
// TODO: move to own file
class PesPacket
{
public:

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

    int64_t pts;
    int64_t dts;



    friend std::ostream& operator<<(std::ostream& ss, const PesPacket& rhs)
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

    // TODO much more information that we don't need for now...

    ByteVector mPesBuffer;
};

/*! @brief Table_id assignment values
 *
 * Table 2-31 - table_id assignment values
 *
 */
enum PsiTableId
{
    PSI_TABLE_ID_PAT = 0x00,       /*! Program Association Table Id */
    PSI_TABLE_ID_CAT = 0x01,       /*! Conditional Access Section Table Id */
    PSI_TABLE_ID_PMT = 0x02,       /*! Program Map Table Id */
    PSI_TABLE_ID_TDT = 0x03,       /*! Transport stream Desciption Table id */
    PSI_TABLE_ID_SDT = 0x04,       /*! ISO_IEC_14496_scene_description_section */
    PSI_TABLE_ID_ODT = 0x05,       /*! ISO_IEC_14496_object_descriptor_section */
    PSI_TABLE_ID_METADATA = 0x06,  /*! Metadata_section */
    PSI_TABLE_ID_IPMP = 0x07,      /*! IPMP Control Information Section */
    PSI_TABLE_ID_14496 = 0x08,     /*! ISO_IEC_14496_section */
    PSI_TABLE_ID_INCOMPLETE = 0xfe, /*! User defined */
    PSI_TABLE_ID_FORBIDDEN = 0xff /*! Forbidden */
};

static std::map<PsiTableId, std::string> PsiTableToString =
        {
                { PSI_TABLE_ID_PAT, "PSI_TABLE_ID_PAT"},
                { PSI_TABLE_ID_CAT, "PSI_TABLE_ID_CAT"},
                { PSI_TABLE_ID_PMT, "PSI_TABLE_ID_PMT"},
                { PSI_TABLE_ID_TDT, "PSI_TABLE_ID_TDT"},
                { PSI_TABLE_ID_SDT, "PSI_TABLE_ID_SDT"},
                { PSI_TABLE_ID_ODT, "PSI_TABLE_ID_ODT"},
                { PSI_TABLE_ID_METADATA, "PSI_TABLE_ID_METADATA"},
                { PSI_TABLE_ID_IPMP, "PSI_TABLE_ID_IPMP"},
                { PSI_TABLE_ID_14496, "PSI_TABLE_ID_14496"},
                { PSI_TABLE_ID_FORBIDDEN, "PSI_TABLE_ID_FORBIDDEN"}};

/*! @brief Stream type
 *
 * Table 2-34 - Stream type assignments.
 *
 */
enum StreamType
{
    STREAMTYPE_RESERVED    = 0x00,
    STREAMTYPE_VIDEO_MPEG1 = 0X01,
    STREAMTYPE_VIDEO_MPEG2 = 0x02,
    STREAMTYPE_AUDIO_MPEG1 = 0X03,
    STREAMTYPE_AUDIO_MPEG2 = 0x04,
    STREAMTYPE_PRIVATE_TABLE = 0x05,
    STREAMTYPE_PRIVATE_PES = 0X06,
    STREAMTYPE_MHEG        = 0X07,
    STREAMTYPE_VIDEO_MPEG4 = 0X10,
    STREAMTYPE_AUDIO_ADTS  = 0x0F,
    STREAMTYPE_VIDEO_H264  = 0x1B,
    STREAMTYPE_VIDEO_H265  = 0X24,
    STREAMTYPE_AUDIO_AC3   = 0X81,
    STREAMTYPE_Any         = 0xFF // User private
};

static std::map<StreamType, std::string> StreamTypeToString =
        {
                { STREAMTYPE_RESERVED, "STREAMTYPE_RESERVED"},
                { STREAMTYPE_VIDEO_MPEG1, "STREAMTYPE_VIDEO_MPEG1"},
                { STREAMTYPE_VIDEO_MPEG2, "STREAMTYPE_VIDEO_MPEG2"},
                { STREAMTYPE_AUDIO_MPEG1, "STREAMTYPE_AUDIO_MPEG1"},
                { STREAMTYPE_AUDIO_MPEG2, "STREAMTYPE_AUDIO_MPEG2"},
                { STREAMTYPE_PRIVATE_TABLE, "STREAMTYPE_PRIVATE_TABLE"},
                { STREAMTYPE_PRIVATE_PES, "STREAMTYPE_PRIVATE_PES"},
                { STREAMTYPE_MHEG, "STREAMTYPE_MHEG"},
                { STREAMTYPE_VIDEO_MPEG4, "STREAMTYPE_VIDEO_MPEG4"},
                { STREAMTYPE_AUDIO_ADTS, "STREAMTYPE_AUDIO_ADTS"},
                { STREAMTYPE_VIDEO_H264, "STREAMTYPE_VIDEO_H264"},
                { STREAMTYPE_VIDEO_H265, "STREAMTYPE_VIDEO_H265"},
                { STREAMTYPE_AUDIO_AC3, "STREAMTYPE_AUDIO_AC3"},
                { STREAMTYPE_Any, "STREAMTYPE_Any"}
        };


/*! @brief Stream_id assignments
 *
 *[ISO 13818-1] Table 2-22 – Stream_id assignments
 *
 */
enum StreamId
{
    program_stream_map = 0xBC,
    private_stream_1 = 0xBD,
    padding_stream = 0xBE,
    private_stream_2 = 0xBF,
    pes_audio_stream = 0xC0,
    pes_video_stream = 0xE0,
    ECM_stream = 0xF0,
    EMM_stream = 0xF1,
    DSMCC_stream = 0xF2,
    ITU_T_Rec_H222_1_type_E_stream = 0xF8,
    program_stream_directory = 0xFF
};