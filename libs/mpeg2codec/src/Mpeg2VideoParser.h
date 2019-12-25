/*****************************************************************
*
*    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
*
*    mpeg2ts - mpeg2ts lib
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
#pragma once
                                                                                                                                                                                                  
#include <cstdint>              // for uint8_t, uint64_t
#include <map>                  // for map, _Rb_tree_const_iterator
#include <string>               // for string, basic_string
#include <utility>              // for pair
#include <vector>               // for vector

// Project files
#include "EsParser.h"           // for EsParser
#include "GetBits.h"            // for GetBits
#include "Mpeg2Codec.h"

namespace mpeg2
{

/*!
 * @brief Mpeg2 codec parser implementation
 */
class Mpeg2VideoEsParser : public GetBits, public EsParser
{
public:
    Mpeg2VideoEsParser(const Mpeg2VideoEsParser& arg) = delete;
    Mpeg2VideoEsParser& operator=(const Mpeg2VideoEsParser& arg) = delete;

    Mpeg2VideoEsParser()
        : EsParser({ 0x00, 0x00, 0x01 })
    {
    }

    virtual ~Mpeg2VideoEsParser() = default;

    /// @brief Parses a binary buffer containing codec data like H262 or H264 and
    /// let the specialization analyze the results.
    /// @param buf The binary data to parse
    std::vector<EsInfoMpeg2> parse(const std::vector<uint8_t>& buf);

    /// @brief Analyze the content on data after startcodes.
    std::vector<EsInfoMpeg2> analyze();

    static std::string toString(Mpeg2Type e);

private:
    static std::map<uint8_t, std::string> AspectToString;
    static std::map<uint8_t, std::string> FrameRateToString;
};


inline std::string Mpeg2VideoEsParser::toString(Mpeg2Type e)
{
    const std::map<Mpeg2Type, std::string> MyEnumStrings{ { Mpeg2Type::Info, "Info" },
                                                          { Mpeg2Type::SliceCode, "SliceCode" },
                                                          { Mpeg2Type::SequenceHeader,
                                                            "SequenceHeader" } };
    auto it = MyEnumStrings.find(e);
    return it == MyEnumStrings.end() ? "Out of range" : it->second;
}
}
