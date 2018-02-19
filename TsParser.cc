/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */
#include "TsParser.h"
#include <stdio.h>
#include <iostream>

void TsParser::parseTsPacketInfo(const uint8_t *packet, TsPacketInfo &outInfo)
{
    resetBits(packet,  TS_PACKET_SIZE - 1);
    TsHeader hdr = parseTsHeader(packet);
    outInfo.pid = hdr.PID;
    outInfo.errorIndicator = hdr.transport_error_indicator;
    outInfo.isPayloadStart = hdr.payload_unit_start_indicator;
    outInfo.hasAdaptationField = checkHasAdaptationField(hdr);

    if (outInfo.hasAdaptationField)
    {
        parseAdaptationFieldData(packet, outInfo);

        if (outInfo.privateDataSize > 0)
            outInfo.hasPrivateData = true;
        else
            outInfo.hasPrivateData = false;
    }
}


bool TsParser::checkSyncByte(const uint8_t* byte)
{
    return TS_PACKET_SYNC_BYTE == byte[0];
}


TsHeader TsParser::parseTsHeader(const uint8_t *packet)
{
    TsHeader hdr;

    hdr.sync_byte = getBits(8);
    hdr.transport_error_indicator = getBits(1);
    hdr.payload_unit_start_indicator = getBits(1);
    hdr.transport_priority = getBits(1);
    hdr.PID = getBits(13);
    hdr.transport_scrambling_control = getBits(2);
    hdr.adaptation_field_control = getBits(2);
    hdr.continuity_counter = getBits(4);
//    std::cout << hdr << std::endl;
    return hdr;
}


bool TsParser::checkHasAdaptationField(TsHeader hdr)
{
    if (hdr.adaptation_field_control == ts_adaptation_field_control_adaptation_only ||
        hdr.adaptation_field_control == ts_adaptation_field_control_adaptation_payload)
    {
        return true;
    }
    else
    {
        return false;
    }
}


uint8_t TsParser::parseAdaptationFieldLength(TsHeader hdr, const uint8_t* packet)
{
    if (checkHasAdaptationField(hdr))
    {
        return packet[TS_PACKET_HEADER_SIZE];
    }
    return 0; // Length must be 0 if ts-packet doesn't have adaptation field
}


TsAdaptationFieldHeader TsParser::parseAdaptationFieldHeader(const uint8_t* packet)
{
    return *reinterpret_cast<const TsAdaptationFieldHeader* >(&packet[TS_PACKET_HEADER_SIZE]);
}


// Following spec Table 2-6 Transport stream adaptation field, see ISO/IEC 13818-1:2015.
void TsParser::parseAdaptationFieldData(const uint8_t* packet, TsPacketInfo& outInfo)
{
    TsAdaptationFieldHeader adaptHdr = parseAdaptationFieldHeader(packet);
    uint16_t offset = TS_PACKET_HEADER_SIZE + TS_PACKET_ADAPTATION_FIELD_SIZE;

    if (adaptHdr.PCR_flag) {
        outInfo.pcr = parsePcr(&packet[offset]);
        offset += PCR_SIZE;
    }
    if (adaptHdr.OPCR_flag) {
        outInfo.opcr = parsePcr(&packet[offset]);
        offset += OPCR_SIZE;
    }
//    if (adaptHdr.splicing_point_flag) {
//        outData.splice_countdown = packet[offset];
//        offset += 1; // size of splice_countdown is 1 byte
//    }
    if (adaptHdr.transport_private_data_flag) {
        outInfo.privateDataSize = packet[offset];
        offset += 1; // size of transport-private_data_length is 1 bytes
        outInfo.privateDataOffset = offset;
    }
}


uint64_t TsParser::parsePcr(const uint8_t* buffer)
{
    uint64_t pcr_base = 0;
    uint16_t pcr_extension = 0;

    resetBits(buffer, TS_PACKET_SIZE); //TODO: set right size

    pcr_base = getBits(33);
    int reserved = getBits(6);

    pcr_extension = getBits(9);
    pcr_base = pcr_base * 300;

    // 9 bits
    pcr_base |= pcr_extension;

    return pcr_base;
}


std::ostream& operator << (std::ostream& ss, const TsHeader& rhs)
{
    ss << "-------------_TsHeader------------- " << std::endl;
    ss << "sync_byte:  0x" << std::hex << (int)rhs.sync_byte << std::dec << std::endl;
    ss << "transport_error_indicator: " << (int)rhs.transport_error_indicator << std::endl;
    ss << "payload_unit_start_indicator: " << (int)rhs.payload_unit_start_indicator << std::endl;
    ss << "transport_priority: " << (int)rhs.transport_priority << std::endl;
    ss << "PID: " << rhs.PID << std::endl;
    ss << "transport_scrambling_control: " << (int)rhs.transport_scrambling_control << std::endl;
    ss << "adaptation_field_control: " << (int)rhs.adaptation_field_control << std::endl;
    ss << "continuity_counter: " << (int)rhs.continuity_counter << std::endl;
    return ss;
}
