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
#include <ostream>    // for operator<<, basic_ostream::operator<<, ostream                                                                  
#include "mpeg2ts.h"  // for PesPacket, operator<<   

namespace mpeg2ts
{

std::ostream& operator<<(std::ostream& ss, const PesPacket& rhs)
{
    ss << "\n-------------PesPacket-------------\n";
    ss << "packet_start_code_prefix:" << std::hex << rhs.packet_start_code_prefix << std::dec << '\n';
    ss << "stream_id: " << static_cast<int>(rhs.stream_id) << '\n';
    ss << "PES_packet_length: " << static_cast<int>(rhs.PES_packet_length) << '\n';

    ss << "PES_scrambling_control: " << static_cast<int>(rhs.PES_scrambling_control) << '\n';
    ss << "PES_priority: " << static_cast<int>(rhs.PES_priority) << '\n';
    ss << "data_alignment_indicator: " << static_cast<int>(rhs.data_alignment_indicator) << '\n';
    ss << "copyright: " << static_cast<int>(rhs.copyright) << '\n';
    ss << "original_or_copy: " << static_cast<int>(rhs.original_or_copy) << '\n';
    ss << "PTS_DTS_flags: " << static_cast<int>(rhs.PTS_DTS_flags) << '\n';
    ss << "ESCR_flag: " << static_cast<int>(rhs.ESCR_flag) << '\n';
    ss << "ES_rate_flag: " << static_cast<int>(rhs.ES_rate_flag) << '\n';
    ss << "DSM_trick_mode_flag: " << static_cast<int>(rhs.DSM_trick_mode_flag) << '\n';
    ss << "additional_copy_info_flag: " << static_cast<int>(rhs.additional_copy_info_flag) << '\n';
    ss << "PES_CRC_flag: " << static_cast<int>(rhs.PES_CRC_flag) << '\n';
    ss << "PES_extension_flag: " << static_cast<int>(rhs.PES_extension_flag) << '\n';

    ss << "pts: " << rhs.pts << '\n';
    ss << "dts: " << rhs.dts << '\n';

    ss << "PES_header_data_length: " << static_cast<int>(rhs.PES_header_data_length) << '\n';
    return ss;
}
}
