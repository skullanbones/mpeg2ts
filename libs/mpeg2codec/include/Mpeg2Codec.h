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

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace mpeg2 {

enum class Mpeg2Type
{
    Info,
    SliceCode,
    SequenceHeader
};

struct EsInfoMpeg2PictureSliceCode
{
    uint64_t picType{ 0 }; // I, B, P
};

struct EsInfoMpeg2SequenceHeader
{
    int width{ 0 }, height{ 0 };
    std::string aspect{ "" };
    std::string framerate{ "" };
};

struct EsInfoMpeg2
{
    Mpeg2Type type;
    int picture{ 0 }; // slice
    std::string msg{ "" };
    EsInfoMpeg2PictureSliceCode slice;
    EsInfoMpeg2SequenceHeader sequence;
};

class Mpeg2VideoEsParser; // forward declaration

/*!
 * @brief Mpeg2 codec parser API
 */
class Mpeg2Codec
{
public:
    Mpeg2Codec();
    ~Mpeg2Codec();

    /*!
     * @brief Parses a binary buffer containing codec MPEG2 data and
     * let the specialization analyze the results.
     * @param buf The binary data to parse
     * @return Codec parsed meta-data
     */
    std::vector<EsInfoMpeg2> parse(const std::vector<uint8_t>& buf);

    /*!
     * @brief Analyze the content on data after startcodes.
     */
    std::vector<EsInfoMpeg2> analyze();

    /*!
     * @brief Convert Mpeg2Type to string
     * @return Mpeg2Type string
     */
    std::string toString(Mpeg2Type e);

private:
    // Non copyable
    Mpeg2Codec(const Mpeg2Codec&);
    Mpeg2Codec& operator=(const Mpeg2Codec&);
    // Non movable
    Mpeg2Codec(Mpeg2Codec&&);
    Mpeg2Codec& operator=(Mpeg2Codec&&);
    
    std::unique_ptr<Mpeg2VideoEsParser> mPimpl;
};


} // namespace mpeg2