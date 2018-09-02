#pragma once

/// Project files
#include "TsStandards.h"

/*!
 * @brief Base class for all PSI tables
 */

#include <ostream>
#include <vector>

class PsiTable
{
public:
    PsiTable()
    {
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

    friend std::ostream& operator<<(std::ostream& ss, const PsiTable& rhs);

    /// @brief Comparison operator for comparing 2 PsiTables
    bool operator==(const PsiTable& rhs) const;

    bool operator!=(const PsiTable& rhs) const;
};

/*!
 * PAT table
 */
class PatTable : public PsiTable
{
public:
    std::vector<Program> programs;

    friend std::ostream& operator<<(std::ostream& ss, const PatTable& rhs);

    /// @brief Comparison operator for comparing 2 PatTables
    bool operator==(const PatTable& rhs) const;

    bool operator!=(const PatTable& rhs) const;
};

struct StreamTypeHeader
{
    StreamType stream_type;
    uint16_t elementary_PID;
    uint16_t ES_info_length;

    friend std::ostream& operator<<(std::ostream& ss, const StreamTypeHeader& rhs);

    bool operator==(const StreamTypeHeader& rhs) const;

    bool operator!=(const StreamTypeHeader& rhs) const;
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


    friend std::ostream& operator<<(std::ostream& ss, const PmtTable& rhs);

    /// @brief Comparison operator for comparing 2 PmtTables
    bool operator==(const PmtTable& rhs) const;

    bool operator!=(const PmtTable& rhs) const;
};

struct CatDescriptor
{
};

class CatTable : public PsiTable
{
public:
    std::vector<CatDescriptor> descriptors;
};
