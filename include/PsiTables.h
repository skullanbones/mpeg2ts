#pragma once

/// Project files
#include "TsStandards.h"

/*!
 * @brief Base class for all PSI tables
 */

#include <iostream>

class PsiTable
{
public:
    PsiTable() {
        table_id = 0;
        section_syntax_indicator = false;
        section_length = 0;
        transport_stream_id = 0;
        version_number = 0;
        current_next_indicator = false;
        section_number = 0;
        last_section_number = 0;
        CRC_32 = 0;
    }

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

    /// @brief Comparison operator for comparing 2 PsiTables
    bool operator==(const PsiTable &rhs) const
    {
        return CRC_32 == rhs.CRC_32 &&
                table_id == rhs.table_id &&
                section_syntax_indicator == rhs.section_syntax_indicator &&
                section_length == rhs.section_length &&
                transport_stream_id == rhs.transport_stream_id &&
                version_number == rhs.version_number &&
                current_next_indicator == rhs.current_next_indicator &&
                section_number == rhs.section_number &&
                last_section_number == rhs.last_section_number;
    }

    bool operator!=(const PsiTable &rhs) const
    {
        return !operator==(rhs);
    }
};

/*!
 * PAT table
 */
class PatTable : public PsiTable
{
public:
    std::vector<Program> programs;

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
            if (rhs.programs[i].type == ProgramType::PMT) {
                ss << "program_map_PID: " << rhs.programs[i].program_map_PID << std::endl;
            } else if (rhs.programs[i].type == ProgramType::NIT) {
                ss << "network_PID: " << rhs.programs[i].network_PID << std::endl;
            }
        }

        return ss;
    }

    /// @brief Comparison operator for comparing 2 PatTables
    bool operator==(const PatTable &rhs) const
    {
        bool psi = PsiTable::operator==(rhs);
        if (psi == false) {
            return false;
        }

        // 1. check number of programs
        if (this->programs.size() != rhs.programs.size())
        {
            std::cout << "PatTable number of programs unequal." << std::endl;
            return false;
        }
        // 2. check content of each programs
        unsigned i = 0;
        for (auto prg : programs)
        {
            if (prg != rhs.programs.at(i)) {
                std::cout << "PatTable programs content unequal for program: " << prg.program_number << std::endl;
                return false;
            }
            i++;
        }
        return true;
    }

    bool operator!=(const PatTable &rhs) const
    {
        return !operator==(rhs);
    }
};

struct StreamTypeHeader
{
    uint8_t stream_type;
    uint16_t elementary_PID;
    uint16_t ES_info_length;

    bool operator==(const StreamTypeHeader &rhs) const
    {
        return stream_type   == rhs.stream_type &&
                elementary_PID == rhs.elementary_PID &&
                ES_info_length == rhs.ES_info_length;
    }

    bool operator!=(const StreamTypeHeader &rhs) const
    {
        return !operator==(rhs);
    }
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

    /// @brief Comparison operator for comparing 2 PmtTables
    bool operator==(const PmtTable &rhs) const
    {
        bool psi = PsiTable::operator==(rhs);
        if (psi == false) {
            return false;
        }

        // 1. First check CRC 32
        if (this->CRC_32 != rhs.CRC_32)
        {
            std::cout << "PmtTable CRC_32 unequal." << std::endl;
            return false;
        }
        // 2. Secondly check PCR_PID
        if (this->PCR_PID != rhs.PCR_PID)
        {
            std::cout << "PmtTable PCR_PID unequal." << std::endl;
            return false;
        }
        // 3. Thirdly check program_info_length
        if (this->program_info_length != rhs.program_info_length)
        {
            std::cout << "PmtTable program_info_length unequal." << std::endl;
            return false;
        }
        // 4. check number of streams
        if (this->streams.size() != rhs.streams.size())
        {
            std::cout << "PmtTable number of streams unequal." << std::endl;
            return false;
        }
        // 5. check content of each streams
        unsigned i = 0;
        for (auto stream : streams)
        {
            if (stream != rhs.streams.at(i)) {
                std::cout << "PmtTable stream content unequal for stream_type: " << stream.stream_type << std::endl;
                return false;
            }
            i++;
        }
        return true;
    }

    bool operator!=(const PmtTable &rhs) const
    {
        return !operator==(rhs);
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
