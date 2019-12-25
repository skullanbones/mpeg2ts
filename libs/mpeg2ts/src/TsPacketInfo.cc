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
#include <ostream>    // for operator<<, ostream, basic_ostream, basic_ostre...

#include "mpeg2ts.h"  // for TsPacketInfo, operator<<

namespace mpeg2ts
{

std::ostream& operator<<(std::ostream& ss, const TsPacketInfo& rhs)
{
    ss << "-------------TsPacketInfo------------- " << '\n';
    ss << "PID: " << rhs.pid << '\n';
    ss << "errorIndicator: " << static_cast<int>(rhs.errorIndicator) << '\n';
    ss << "isPayloadStart: " << static_cast<int>(rhs.isPayloadStart) << '\n';
    ss << "hasAdaptationField: " << rhs.hasAdaptationField << '\n';
    ss << "hasPayload: " << rhs.hasPayload << '\n';
    ss << "hasPrivateData: " << rhs.hasPrivateData << '\n';
    ss << "hasRandomAccess: " << rhs.hasRandomAccess << '\n';
    ss << "isScrambled: " << rhs.isScrambled << '\n';
    ss << "isDiscontinuity: " << rhs.isDiscontinuity << '\n';

    ss << "pcr: " << rhs.pcr << '\n';
    ss << "opcr: " << rhs.opcr << '\n';
    if (rhs.hasPrivateData)
    {
        ss << "privateDataSize: " << rhs.privateDataSize << '\n';
        ss << "privateDataOffset: " << rhs.privateDataOffset << '\n';
    }
    if (rhs.hasPayload)
    {
        ss << "payloadSize: " << rhs.payloadSize << '\n';
        ss << "payloadStartOffset: " << static_cast<int>(rhs.payloadStartOffset) << '\n';
    }
    ss << "isError: " << rhs.isError << '\n';
    return ss;
}

} // namespace mpeg2ts
