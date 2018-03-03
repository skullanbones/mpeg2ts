/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */
#pragma once

#include <sstream>
#include <stdint.h>
#include <stdio.h>


class TsPacketInfo
{
public:
    uint16_t pid; // This Packet Identifier.

    bool errorIndicator;     // If indication of at least 1 uncorrectable bit in ts-packet
    bool isPayloadStart;     // If this packet is the first in a PES-packet.
    bool hasAdaptationField; // If this packet contains adaptation field data.
    bool hasPayload;         // If this packet contains payload.
    bool hasPrivateData;     // If this packet contains private data.
    bool hasRandomAccess;    // If this packet contain aid random access.
    bool isScrambled;        // If this packet is scrambled

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


    std::string toString() const
    {
        std::ostringstream ss;
        ss << "-------------TsPacketInfo------------- " << std::endl;
        ss << "PID: " << pid << std::endl;
        ss << "hasAdaptationField: " << hasAdaptationField << std::endl;
        ss << "pcr: " << pcr << std::endl;
        ss << "opcr: " << opcr << std::endl;
        if (hasPrivateData)
        {
            ss << "privateDataSize: " << privateDataSize << std::endl;
            ss << "privateDataOffset: " << privateDataOffset << std::endl;
        }
        if (hasPayload)
        {
            ss << "payloadSize: " << payloadSize << std::endl;
            ss << "payloadStartOffset: " << (int)payloadStartOffset << std::endl;
        }
        return ss.str();
    }
};
