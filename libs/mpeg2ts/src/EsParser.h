
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
#ifndef _ESPARSER_H
#define _ESPARSER_H

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <vector>


struct EsInfo
{
    virtual ~EsInfo(){};
};

class EsParser
{
public:
    EsParser(std::vector<uint8_t> a_startCode)
        : m_startCode{ a_startCode }
    {
    }

    virtual ~EsParser() = default;


    /// @brief Finds startcode in a binary buffer by using std search algorithm
    /// @param buf The binary data to find startcodes in
    std::vector<std::size_t> findStartCodes(const std::vector<uint8_t>& buf);

protected:
    std::vector<uint8_t> mPicture;
    std::vector<uint8_t> m_startCode;
};

inline std::vector<std::size_t> EsParser::findStartCodes(const std::vector<uint8_t>& a_buf)
{
    std::vector<std::size_t> indexes{};
    auto it{ a_buf.begin() };
    while ((it = std::search(it, a_buf.end(), m_startCode.begin(), m_startCode.end())) != a_buf.end())
    {
        indexes.push_back(std::distance(a_buf.begin(), it++));
    }
    return indexes;
}

#endif /* _ESPARSER_H */
