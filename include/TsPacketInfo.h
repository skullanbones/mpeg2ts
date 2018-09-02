/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */
#pragma once

#include <cstdint>

// Project files
#include "TsStandards.h" // For TsHeader


class TsPacketInfo
{
public:
    uint16_t pid; // This Packet Identifier.
    TsHeader hdr; // This packet Ts Header

    bool errorIndicator;     // If indication of at least 1 uncorrectable bit in ts-packet
    bool isPayloadStart;     // If this packet is the first in a PES-packet.
    bool hasAdaptationField; // If this packet contains adaptation field data.
    bool hasPayload;         // If this packet contains payload.
    bool hasPrivateData;     // If this packet contains private data.
    bool hasRandomAccess;    // If this packet contain aid random access.
    bool isScrambled;        // If this packet is scrambled
    bool isDiscontinuity;    // If this packet has discontinuity flag set

    uint8_t continuityCounter : 4; // 4-bit continuity counter

    // Adaptation field data:
    uint64_t pcr; // PCR clock...
    uint64_t opcr;

    // For elementary-streams
    uint64_t pts;
    uint64_t dts;

    // Private data:
    uint32_t privateDataSize;
    uint32_t privateDataOffset;

    // Payload data:
    size_t payloadSize;         // The size of the payload
    uint8_t payloadStartOffset; // Offset from and sync byte to start of payload.

    bool isError; // If a parser error or TS not following standards.


    friend std::ostream& operator<<(std::ostream& ss, const TsPacketInfo& rhs);
};
