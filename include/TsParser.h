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

#include <cstdint>

// Project files
#include "GetBits.h"
#include "PesPacket.h"
#include "PsiTables.h"
#include "TsPacketInfo.h"
#include "TsStandards.h"
#include "TsStatistics.h"


class TsParser : GetBits, public TsStatistics
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
    void collectTable(const uint8_t* tsPacket, const TsPacketInfo& tsPacketInfo, uint8_t& table_id);
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
    PatTable parsePatPacket();

    /*!
     * Parse PMT table
     * @param packet
     * @param info
     * @return
     */
    PmtTable parsePmtPacket();

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
    void parsePesPacket(int16_t pid);

private:
    ByteVector mSectionBuffer;
    std::map<uint16_t, PesPacket> mPesPacket;
    uint64_t mPacketErrorCounter;              // Wrong sync byte
    uint64_t mPacketDiscontinuityErrorCounter; // Wrong continuity
};
