//
// Created by microlab on 2/24/18.
//

#pragma once

/// @brief Base class for all tables
/* Taken from ISO-IEC 13818-1 Table 2-30 – Program association section */
// TODO: move to own file
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

    // TODO use union also
    uint16_t program_number;
    uint16_t network_PID;
    uint16_t program_map_PID;

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