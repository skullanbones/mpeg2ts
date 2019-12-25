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
#include <iterator>       // for end, begin
#include <new>            // for bad_alloc
#include <sstream>        // for operator<<, ostringstream, basic_ostream

// 3rd-party
#include <plog/Log.h>     // for LOGE
#include "plog/Record.h"  // for Record

// Project files
#include "Mpeg2VideoParser.h"

namespace mpeg2
{


std::vector<EsInfoMpeg2> Mpeg2VideoEsParser::parse(const std::vector<uint8_t>& a_buf)
{
    std::vector<EsInfoMpeg2> ret;

    std::vector<std::size_t> startCodes = findStartCodes(a_buf);

    // No startcodes -> no parsing
    if (startCodes.size() == 0)
        return ret;

    // There is nothing to parse if the frame only contains a NAL startcode
    if (a_buf.size() <= m_startCode.size())
        return ret;

    for (std::size_t ind = 0; ind < startCodes.size(); ++ind)
    {
        // create sub vector
        try
        {
            mPicture.clear();
            std::vector<uint8_t>::const_iterator first =
            a_buf.begin() + startCodes[ind] + m_startCode.size(); // skip start code
            std::vector<uint8_t>::const_iterator last;
            // the last startcode is a corner case
            // also only have 1 startcode is a corner case
            if (ind == (startCodes.size() - 1) || startCodes.size() == 1)
            {
                last = a_buf.end();
            }
            else
            {
                last = a_buf.begin() + startCodes[ind + 1];
            }

            std::vector<uint8_t> newVec(first, last);
            mPicture = newVec;

            std::vector<EsInfoMpeg2> b = analyze();
            ret.insert(std::end(ret), std::begin(b), std::end(b));
        }
        catch (std::bad_alloc& e)
        {
            LOGE << "std::Exception what: %s\n" << e.what();
        }
    }

    return ret;
}


std::vector<EsInfoMpeg2> Mpeg2VideoEsParser::analyze()
{
    resetBits(mPicture.data(), mPicture.size());
    std::ostringstream msg;

    EsInfoMpeg2 info;
    std::vector<EsInfoMpeg2> ret;

    info.type = Mpeg2Type::Info;
    info.picture = mPicture[0];
    if (info.picture == 0 && mPicture.size() > 4)
    {
        info.type = Mpeg2Type::SliceCode;
        skipBits(10 + 8);
        info.slice.picType = static_cast<int>(getBits(3));
        switch (info.slice.picType)
        {
        case 1:
            msg << "I";
            break;
        case 2:
            msg << "P";
            break;
        case 3:
            msg << "B";
            break;
        default:
            msg << "forbiden/reserved";
        };
        info.msg = msg.str();
    }
    else if (info.picture >= 0x01 && info.picture <= 0xaf)
    {
        info.msg = "slice_start_code";
    }
    else if (info.picture == 0xb0 && info.picture == 0xb1 && info.picture == 0xb6)
    {
        info.msg = "reserved";
    }
    else if (info.picture == 0xb2)
    {
        info.msg = "user_data_start_code";
    }
    else if (info.picture == 0xb3)
    {
        info.type = Mpeg2Type::SequenceHeader;
        info.msg = "sequence_header_code ";
        skipBits(8);
        info.sequence.width = static_cast<int>(getBits(12));
        info.sequence.height = static_cast<int>(getBits(12));
        uint8_t aspect_ratio_information = static_cast<uint8_t>(getBits(4));
        uint8_t frame_rate_code = static_cast<uint8_t>(getBits(4));
        info.sequence.aspect = AspectToString[aspect_ratio_information];
        info.sequence.framerate = FrameRateToString[frame_rate_code];
    }
    else if (info.picture == 0xb4)
    {
        info.msg = "sequence_error_code";
    }
    else if (info.picture == 0xb5)
    {
        info.msg = "extension_start_code";
    }
    else if (info.picture == 0xb7)
    {
        info.msg = "sequence_end_code";
    }
    else if (info.picture == 0xb8)
    {
        info.msg = "group_start_code";
    }
    else
    {
        info.msg = "system start code";
    }
    ret.push_back(std::move(info));
    return ret;
}

std::map<uint8_t, std::string> Mpeg2VideoEsParser::AspectToString =
{ { 0, "forbiden" }, { 1, "square" },   { 2, "3x4" },      { 3, "9x16" },
  { 4, "1x121" },    { 5, "reserved" }, { 6, "reserved" }, { 7, "reserved" } };

std::map<uint8_t, std::string> Mpeg2VideoEsParser::FrameRateToString =
{ { 0, "forbiden" },  { 1, "23.97" },     { 2, "24" },        { 3, "25" },
  { 4, "29.97" },     { 5, "30" },        { 6, "50" },        { 7, "59.94" },
  { 8, "60" },        { 9, "reserved" },  { 10, "reserved" }, { 11, "reserved" },
  { 12, "reserved" }, { 13, "reserved" }, { 14, "reserved" }, { 15, "reserved" } };
}
