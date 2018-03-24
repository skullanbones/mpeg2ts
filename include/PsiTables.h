#pragma once

/// Project files
#include "TsStandards.h"

/*!
 * @brief Base class for all tables
 */
/* Taken from ISO-IEC 13818-1 Table 2-30 – Program association section */
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

/*!
 * PAT table
 */
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

/*!
 * PMT Table.
 */
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
            ss << "stream_type: " << StreamTypeToString[(int)rhs.streams[i].stream_type] << ",  ("
               << (int)rhs.streams[i].stream_type << ")" << std::endl;
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
