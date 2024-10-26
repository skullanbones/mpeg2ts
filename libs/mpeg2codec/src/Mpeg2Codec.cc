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
#include <memory>


#include "Mpeg2Codec.h"

#include "Mpeg2VideoParser.h"

namespace mpeg2
{

Mpeg2Codec::Mpeg2Codec()
    : mPimpl(new Mpeg2VideoEsParser())
{
}

Mpeg2Codec::~Mpeg2Codec()
{
}


std::vector<EsInfoMpeg2> Mpeg2Codec::parse(const std::vector<uint8_t>& buf)
{
    return mPimpl->parse(buf);
}

/// @brief Analyze the content on data after startcodes.
std::vector<EsInfoMpeg2> Mpeg2Codec::analyze()
{
    return mPimpl->analyze();
}

std::string Mpeg2Codec::toString(Mpeg2Type e)
{
    return mPimpl->toString(e);
}

} // namespace mpeg2