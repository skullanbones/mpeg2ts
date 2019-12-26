/*****************************************************************
*
*    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
*
*    mpeg2ts - mpeg2ts h264 lib
*
*    This file is part of mpeg2ts (Mpeg2 Transport Stream Library).
*
*    Unless you have obtained mpeg2ts under a different license,
*    this version of mpeg2ts is mpeg2ts|GPL.
*    Mpeg2ts|GPL is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License as
*    published by the Free Software Foundation; either version 2,
*    or (at your option) any later version.
*
*    Mpeg2ts|GPL is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with mpeg2ts|GPL; see the file COPYING.  If not, write to the
*    Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
*    02111-1307, USA.
*
********************************************************************/
#ifndef _H264PARSER_H
#define _H264PARSER_H

#include <cstdint>              // for uint8_t
#include <iosfwd>               // for size_t
#include <map>                  // for map, _Rb_tree_const_iterator
#include <string>               // for string, basic_string
#include <utility>              // for pair
#include <vector>               // for vector

/// Project files
#include "EsParser.h"           // for EsParser
#include "GetBits.h"            // for GetBits
#include "H264Codec.h"


namespace h264
{


class H264EsParser : public EsParser, public GetBits
{
public:
    H264EsParser(const H264EsParser& arg) = delete;
    H264EsParser& operator=(const H264EsParser& arg) = delete;
    H264EsParser()
        : EsParser({ 0x00, 0x00, 0x00, 0x01 })
        , log2_max_frame_num_minus4(0)
        , separate_colour_plane_flag(0)
        , frame_mbs_only_flag(0)
    {
    }

    virtual ~H264EsParser() = default;

    /// @brief Parses a binary buffer containing codec data like H262 or H264 and
    /// let the specialization analyze the results.
    /// @param buf The binary data to parse
    std::vector<EsInfoH264> parse(const std::vector<uint8_t>& buf);

    /// @brief Analyze the content on data after startcodes.
    std::vector<EsInfoH264> analyze();

    static std::string toString(H264InfoType e);
    static std::string toString(NalUnitType e);

private:
    std::string seipayloadTypeToString(uint64_t payloadType);

    void slice_header(NalUnitType nal_unit_type, EsInfoH264& info);
    uint64_t getBitsDecodeUGolomb();
    void scaling_list(uint8_t* scalingList, std::size_t sizeOfScalingList);
    void seq_parameter_set_rbsp(NalUnitType nal_unit_type, EsInfoH264& info);
    void pic_parameter_set_rbsp(NalUnitType nal_unit_type, EsInfoH264& info);

    void parse_vui();

    // sps data
    uint8_t log2_max_frame_num_minus4;
    uint64_t separate_colour_plane_flag;
    uint64_t frame_mbs_only_flag;
};

inline std::string H264EsParser::toString(H264InfoType e)
{
    const std::map<H264InfoType, std::string> MyEnumStrings{
        { H264InfoType::Info, "Info" },
        { H264InfoType::SliceHeader, "SliceHeader" },
        { H264InfoType::SequenceParameterSet, "SequenceParameterSet" },
        { H264InfoType::PictureParameterSet, "PictureParameterSet" }
    };
    auto it = MyEnumStrings.find(e);
    return it == MyEnumStrings.end() ? "Out of range" : it->second;
}

inline std::string H264EsParser::toString(NalUnitType e)
{
    const std::map<NalUnitType, std::string> MyEnumStrings{
        { NalUnitType::Unspecified, "Unspecified" },
        { NalUnitType::Coded_slice_of_a_non_IDR_picture, "Coded_slice_of_a_non_IDR_picture" },
        { NalUnitType::Coded_slice_data_partition_A, "Coded_slice_data_partition_A" },
        { NalUnitType::Coded_slice_data_partition_B, "Coded_slice_data_partition_B" },
        { NalUnitType::Coded_slice_data_partition_C, "Coded_slice_data_partition_C" },
        { NalUnitType::Coded_slice_of_an_IDR_picture, "Coded_slice_of_an_IDR_picture" },
        { NalUnitType::Supplemental_enhancement_information_SEI,
          "Supplemental_enhancement_information_SEI" },
        { NalUnitType::Sequence_parameter_set, "Sequence_parameter_set" },
        { NalUnitType::Picture_parameter_set, "Picture_parameter_set" },
        { NalUnitType::Access_unit_delimiter, "Access_unit_delimiter" },
        { NalUnitType::End_of_sequence, "End_of_sequence" },
        { NalUnitType::End_of_stream, "End_of_stream" },
        { NalUnitType::Filler_data, "Filler_data" }
    };
    auto it = MyEnumStrings.find(e);
    return it == MyEnumStrings.end() ? "Out of range" : it->second;
}
}

#endif /* _H264PARSER_H */
