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

#include "GetBits.h"
#include "TsPacketInfo.h"
#include "TsStandards.h"
#include <stdint.h>


class TsParser : GetBits
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
     * @param packet Pointer to ts-packet.
     * @return True if it has adaptation field, else false.
     */
    bool checkHasAdaptationField(TsHeader header);

    /*!
     * Checks if a ts-packet has the adaptation field.
     * @param packet Pointer to ts-packet.
     * @return True if it has payload, else false.
     */
    bool checkHasPayload(TsHeader header);

    /*!
     * Return a copy of the TsAdaptationFieldHeader
     * @param packet Pointer to ts-packet.
     * @return TsAdaptationFieldHeader
     */
    TsAdaptationFieldHeader parseAdaptationFieldHeader(const uint8_t* packet);

    /*!
     * Parse adaptation field data acoording to ISO/IEC 13818-1:2015
     * @param packet Pointer to ts-packet.
     * @param outInfo Return TsPacketInfo
     */
    void parseAdaptationFieldData(const uint8_t* packet, TsPacketInfo& outInfo);

    /*!
     * Parse PCR from a ts-packet
     * @param ptr Pointer to start of PCR adaptation field.
     * @return The PCR value.
     */
    uint64_t parsePcr(const uint8_t* buffer);

    /*!
     * Parses PSI table
     * @param packet
     * @param info
     * @param psiTable
     */
    void parsePsiTable(const uint8_t* packet, const TsPacketInfo& info, PsiTable& psiTable);

    /*!
     * Parses PAT table
     * @param packet
     * @param info
     * @return
     */
    PatTable parsePatPacket(const uint8_t* packet, const TsPacketInfo& info);

    /*!
     * Parse PMT table
     * @param packet
     * @param info
     * @return
     */
    PmtTable parsePmtPacket(const uint8_t* packet, const TsPacketInfo& info);

private:
    uint64_t mPacketErrorCounter;              // Wrong sync byte
    uint64_t mPacketDiscontinuityErrorCounter; // Wrong continuity
};
