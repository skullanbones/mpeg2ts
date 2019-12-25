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
#include <map>                  // for map

// Project files
#include "GetBits.h"            // for GetBits
#include "TsStatistics.h"       // for TsStatistics
#include "Ts_IEC13818-1.h"      // for TsAdaptationFieldHeader, TsHeader
#include "mpeg2ts.h"            // for ByteVector, TsPacketInfo (ptr only)

namespace mpeg2ts
{

class TsParser : private GetBits
{
public:
    /*!
     * Parse information about one ts-packet to find useful information
     * like for example PES-start, PCR, DTS,
     * @param pkt Pointer to ts-packet.
     * @param info Ts-packet information describing this packet.
     */
    void parseTsPacketInfo(const uint8_t* packet, TsPacketInfo& outInfo);

    /*!
     * Checks if the ts-packet has correct sync byte.
     * @param packet Pointer to Ts-packet first byte.
     * @return True if valid sync byte, else false
     */
    bool checkSyncByte(const uint8_t* byte);

    /*!
     * Return a copy of the TsHeader.
     * @param packet Pointer to ts-packet.
     * @return TsHeader.
     */
    TsHeader parseTsHeader(const uint8_t* packet);

    /*!
     * Checks if a pts-packet has the adaptation field.
     * @param hdr TsPacket header
     * @return True if it has adaptation field, else false.
     */
    bool checkHasAdaptationField(TsHeader hdr);

    /*!
     * Checks if a ts-packet has the adaptation field.
     * @param hdr TsPacket header
     * @return True if it has payload, else false.
     */
    bool checkHasPayload(TsHeader hdr);

    /*!
     * Return a copy of the TsAdaptationFieldHeader
     * @return TsAdaptationFieldHeader
     */
    TsAdaptationFieldHeader parseAdaptationFieldHeader();

    /*!
     * Parse adaptation field data acoording to ISO/IEC 13818-1:2015
     * @param packet Pointer to ts-packet.
     * @param outInfo Return TsPacketInfo
     */
    void parseAdaptationFieldData(const uint8_t* packet, TsPacketInfo& outInfo);

    /*!
     * Parse PCR from a ts-packet
     * @return The PCR value.
     */
    uint64_t parsePcr();

    /*!
     * Parses Collects all parts of table and parses basic table information (eg table id)
     * @param tsPacket mpeg2 transport stream packet with table in payload
     * @param tsPacketInfo Input packet inforamtion
     * @param table_id Collected table id
     */
    void collectTable(const uint8_t* tsPacket, const TsPacketInfo& tsPacketInfo, int& table_id, int64_t origin = -1);
    /*!
     * Parses PSI table
     * @param packet
     * @param info
     * @param psiTable
     */
    void parsePsiTable(const ByteVector& table, PsiTable& tableInfo);

    /*!
     * Parses PAT table
     * @param packet
     * @param info
     * @return
     */
    PatTable parsePatPacket(int pid);

    /*!
     * Parse PMT table
     * @param packet
     * @param info
     * @return
     */
    PmtTable parsePmtPacket(int pid);

    /*!
     * Collects several TS-Packets for assembling a complete PES-Packet.
     * When collected this function returns true, during collection (un-complete PES)
     * it returns false. No errors are considered at the moment.
     * @param tsPacket The packet to collect.
     * @param tsPacketInfo Pre-parsed metadata about this TS-Packet.
     * @param pesPacket collected PES but only ready/complete/collected when true
     * @return True if collected a complete PES-Packet false in all other cases
     */
    bool collectPes(const uint8_t* tsPacket, const TsPacketInfo& tsPacketInfo, PesPacket& pesPacket, int64_t origin = -1);

    /*!
     * Parses the start of a new PES-Packet. This is typically done before collecting
     * several TS-Packets for generating a complete PES-Packet. This function is used
     * internally by collectPes().
     */
    void parsePesPacket(int pid);

    /*!
     * Return raw bytes of table at pid
     */
    ByteVector& getRawTable(int pid);

    int64_t getOrigin(int pid)
    {
        return mOrigins[pid];
    }
    TsStatistics mStatistics;

private:
    // TODO maybe 1 parser per pid?
    std::map<int, ByteVector> mSectionBuffer;
    std::map<int, int> mSectionLength;
    std::map<int, int> mTableId;
    std::map<int, int> mReadSectionLength;
    std::map<int, PesPacket> mPesPacket;
    std::map<int, int64_t> mOrigins;
};

} // namespace mpeg2ts
