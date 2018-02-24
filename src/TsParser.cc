/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */
#include "TsParser.h"
#include <iostream>
#include <stdio.h>

void TsParser::parseTsPacketInfo(const uint8_t* packet, TsPacketInfo& outInfo)
{
    resetBits(packet, TS_PACKET_SIZE);
    TsHeader hdr = parseTsHeader(packet);
    outInfo.pid = hdr.PID;
    outInfo.errorIndicator = hdr.transport_error_indicator;
    outInfo.isPayloadStart = hdr.payload_unit_start_indicator;
    outInfo.hasAdaptationField = checkHasAdaptationField(hdr);
    outInfo.hasPayload = checkHasPayload(hdr);

    if (outInfo.hasAdaptationField)
    {
        parseAdaptationFieldData(packet, outInfo);

        if (outInfo.privateDataSize > 0)
            outInfo.hasPrivateData = true;
        else
            outInfo.hasPrivateData = false;
    }
    if (outInfo.hasPayload)
    {
        outInfo.payloadSize = TS_PACKET_SIZE - getByteInx();
        outInfo.payloadStartOffset = getByteInx();
    }
}


bool TsParser::checkSyncByte(const uint8_t* byte)
{
    return TS_PACKET_SYNC_BYTE == byte[0];
}


TsHeader TsParser::parseTsHeader(const uint8_t* packet)
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

bool TsParser::checkHasPayload(TsHeader hdr)
{
    if (hdr.adaptation_field_control == ts_adaptation_field_control_payload_only ||
        hdr.adaptation_field_control == ts_adaptation_field_control_adaptation_payload)
    {
        return true;
    }
    else
    {
        return false;
    }
}

TsAdaptationFieldHeader TsParser::parseAdaptationFieldHeader(const uint8_t* packet)
{
    TsAdaptationFieldHeader ret;
    ret.adaptation_field_length = getBits(8);
    if (ret.adaptation_field_length == 0)
    {
        return ret;
    }
    ret.discontinuity_indicator = getBits(1);
    ret.random_access_indicator = getBits(1);
    ret.elementary_stream_priority_indicator = getBits(1);
    ret.PCR_flag = getBits(1);
    ret.OPCR_flag = getBits(1);
    ret.splicing_point_flag = getBits(1);
    ret.transport_private_data_flag = getBits(1);
    ret.adaptation_field_extension_flag = getBits(1);

    return ret;
}


// Following spec Table 2-6 Transport stream adaptation field, see ISO/IEC 13818-1:2015.
void TsParser::parseAdaptationFieldData(const uint8_t* packet, TsPacketInfo& outInfo)
{
    TsAdaptationFieldHeader adaptHdr = parseAdaptationFieldHeader(nullptr);
    printf("AF len: %d\n", adaptHdr.adaptation_field_length);
    if (adaptHdr.adaptation_field_length == 0)
    {
        return;
    }

    auto ofsAfterAF =
    getByteInx() - 1 + adaptHdr.adaptation_field_length; //-1 8 flags in TsAdaptationFieldHeader

    if (adaptHdr.PCR_flag)
    {
        outInfo.pcr = parsePcr(nullptr);
    }
    if (adaptHdr.OPCR_flag)
    {
        outInfo.opcr = parsePcr(nullptr);
    }
    if (adaptHdr.splicing_point_flag)
    {
        /*outData.splice_countdown = */ getBits(8);
    }

    if (adaptHdr.transport_private_data_flag)
    {
        outInfo.privateDataSize = getBits(8);
        outInfo.privateDataOffset = getByteInx();
        for (auto i = 0; i < outInfo.privateDataSize; i++) // skip it for now
        {
            getBits(8);
        }
    }

    if (adaptHdr.adaptation_field_extension_flag)
    {
        auto adaptation_field_extension_length = getBits(8);
        for (auto i = 0; i < adaptation_field_extension_length; i++) // skip it for now
        {
            getBits(8);
        }
    }

    // 0..N stuffing bytes goes here and we have to adjust read offset
    resetBits(packet, TS_PACKET_SIZE, ofsAfterAF);
}


uint64_t TsParser::parsePcr(const uint8_t* buffer)
{
    uint64_t pcr_base = 0;
    uint64_t pcr_extension = 0;

    pcr_base = getBits(33);
    int reserved = getBits(6);

    pcr_extension = getBits(9);
    pcr_base = pcr_base * 300;

    // 9 bits
    pcr_base += pcr_extension;

    return pcr_base;
}


std::ostream& operator<<(std::ostream& ss, const TsHeader& rhs)
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
