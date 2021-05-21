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
#ifndef _GETBITS_H
#define _GETBITS_H

#include <cstdint>  // for uint8_t, uint64_t                                                                                                                                                                                                      
#include <iosfwd>               // for size_t                                                                        
#include <stdexcept>            // for runtime_error                                                            
#include <string>               // for string   

class GetBits
{
public:
    GetBits();

    virtual ~GetBits() = default;
    /*!
     * Parses maximum 64 bits by bit from data and returns results
     * * @note NOTE! This function does only parse up to 64 bits.
     * Passing a request greater than 64 bits will trigger an out of bound
     * exception.
     * @param requestedBits Number of bits to parse
     * @param data Data to parse
     * @return Parsed bits
     */
    uint64_t getBits(int a_requestedBits);

    /*!
     * Resets bit reader to start over reading from a buffer with
     * start index and buffer size as parameters settings.
     * @param srcBytes The buffer to read from
     * @param srcSize The buffer size to read from
     * @param inx The start byte offset to start read from
     */
    void resetBits(const uint8_t* a_srcBytes, std::size_t a_srcSize, std::size_t a_inx = 0);

    /*!
     * Skips amount of bits of any size. This function can skip any number
     * of bits as long as its inside the data scope, otherwise it will trigger
     * an out of bound exception.
     * @param skipBits Skip amount of bits.
     */
    void skipBits(int a_skipBits);

    /*!
     * Return offset to current byte
     */
    std::size_t getByteInx() const;

    /*!
     * Skips entire bytes instead of bits. Good to use when skip large block of data.
     * @param skipBytes Number of bytes to skip.
     */
    void skipBytes(int a_skipBytes);

    /*!
     * For debugging the data in store up to current parsed index.
     */
    void printSrcBytes() const;

protected:
    uint8_t mNumStoredBits;
    uint8_t mBitStore;
    std::size_t mSrcInx;
    std::size_t mSize;
    const uint8_t* mSrcBytes;
};

class GetBitsException : public std::runtime_error
{
public:
    GetBitsException(const std::string msg)
        : std::runtime_error(msg)
    {
    }

    virtual ~GetBitsException() = default;
};

#endif /* _GETBITS_H */
