/**
 * @class TsParser
 *
 * @brief Parse a transport stream packet
 *
 * @author skullanbones
 *
 * @version $Revision: 0.1 $
 *
 * @date $Date: 2018/02/17 14:16:20 $
 *
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */
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
    void collectTable(const uint8_t* tsPacket, const TsPacketInfo& tsPacketInfo, int& table_id);
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
    bool collectPes(const uint8_t* tsPacket, const TsPacketInfo& tsPacketInfo, PesPacket& pesPacket);

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
    TsStatistics mStatistics;

private:
    // TODO maybe 1 parser per pid?
    std::map<int, ByteVector> mSectionBuffer;
    std::map<int, int> mSectionLength;
    std::map<int, int> mTableId;
    std::map<int, int> mReadSectionLength;
    std::map<int, PesPacket> mPesPacket;
};

} // namespace mpeg2ts
