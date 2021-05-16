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
#include <stdio.h>  // for printf, size_t

#include "GetBits.h"                                                                                                                                  

GetBits::GetBits()
    : mNumStoredBits{ 0 }
    , mBitStore{ 0 }
    , mSrcInx{ 0 }
    , mSize{ 0 }
    , mSrcBytes{ nullptr }
{
}

uint64_t GetBits::getBits(int a_requestedBits)
{
    uint64_t ret{ 0 };

    if (mSrcBytes == nullptr)
    {
        throw GetBitsException("null input data");
    }

    if (a_requestedBits > 64)
    {
        throw GetBitsException("Cannot parse more than 64 individual bits at a time.");
    }

    while (a_requestedBits > 0)
    {
        if (mNumStoredBits == 0)
        {
            if (mSrcInx >= mSize)
            {
                throw GetBitsException("getBits: Out of bound read");
            }

            mNumStoredBits = 8;
            mBitStore = mSrcBytes[mSrcInx++];
        }

        int bitsToFromStore = mNumStoredBits > a_requestedBits ? a_requestedBits : mNumStoredBits;
        ret = (ret << bitsToFromStore) | (mBitStore >> (8 - bitsToFromStore));

        a_requestedBits -= bitsToFromStore;
        mNumStoredBits = static_cast<uint8_t>(mNumStoredBits - bitsToFromStore);
        mBitStore = static_cast<uint8_t>(mBitStore << bitsToFromStore);
    }

    return ret;
}

void GetBits::resetBits(const uint8_t* a_srcBytes, std::size_t a_srcSize, size_t a_inx)
{
    mNumStoredBits = 0;
    mBitStore = 0;
    mSrcInx = a_inx;
    mSize = a_srcSize;
    mSrcBytes = a_srcBytes;
}

void GetBits::skipBits(int a_skipBits)
{
    if (a_skipBits <= 64)
    {
        getBits(a_skipBits);
        return;
    }

    int n{ a_skipBits / 64 };
    int rem{ a_skipBits % 64 };

    for (int i = 0; i < n; ++i)
    {
        mNumStoredBits = 0;
        mBitStore = 0;
        mSrcInx += 8;

        if (mSrcInx >= mSize)
        {
            throw GetBitsException("skipBits: Out of bound read");
        }
    }

    getBits(rem);
}

void GetBits::skipBytes(int a_skipBytes)
{
    if ((mSrcInx + a_skipBytes) >= mSize)
    {
        throw GetBitsException("getBits: Out of bound read mSrcInx: " + std::to_string(mSrcInx));
    }
    else
    {
        mNumStoredBits = 0;
        mBitStore = 0;
        mSrcInx += a_skipBytes;
    }
}

std::size_t GetBits::getByteInx() const
{
    return mNumStoredBits == 0 ? mSrcInx : mSrcInx - 1;
}


void GetBits::printSrcBytes() const
{
    for (std::size_t i = mSrcInx; i < mSize; ++i)
    {
        printf("%02X", mSrcBytes[i]);
    }
    printf("\n");
}
