/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */
#include "TsParser.h"
#include <iostream>
#include <stdio.h>

void TsParser::parseTsPacketInfo(const uint8_t* packet, TsPacketInfo& outInfo)
{
    TsPacketInfo zero = { 0 };
    outInfo = zero;

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
    resetBits(packet, TS_PACKET_SIZE);

    hdr.sync_byte = getBits(8);
    hdr.transport_error_indicator = getBits(1);
    hdr.payload_unit_start_indicator = getBits(1);
    hdr.transport_priority = getBits(1);
    hdr.PID = getBits(13);
    hdr.transport_scrambling_control = getBits(2);
    hdr.adaptation_field_control = getBits(2);
    hdr.continuity_counter = getBits(4);
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
    TsAdaptationFieldHeader hdr;
    hdr.adaptation_field_length = getBits(8);
    if (hdr.adaptation_field_length == 0)
    {
        return hdr;
    }
    hdr.discontinuity_indicator = getBits(1);
    hdr.random_access_indicator = getBits(1);
    hdr.elementary_stream_priority_indicator = getBits(1);
    hdr.PCR_flag = getBits(1);
    hdr.OPCR_flag = getBits(1);
    hdr.splicing_point_flag = getBits(1);
    hdr.transport_private_data_flag = getBits(1);
    hdr.adaptation_field_extension_flag = getBits(1);

    return hdr;
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
        for (uint32_t i = 0; i < outInfo.privateDataSize; i++) // skip it for now
        {
            getBits(8);
        }
    }

    if (adaptHdr.adaptation_field_extension_flag)
    {
        uint8_t adaptation_field_extension_length = getBits(8);
        for (uint8_t i = 0; i < adaptation_field_extension_length; i++) // skip it for now
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
    getBits(6); // reserved

    pcr_extension = getBits(9);
    pcr_base = pcr_base * 300;

    // 9 bits
    pcr_base += pcr_extension;

    return pcr_base;
}


void TsParser::parsePsiTable(const uint8_t* packet, const TsPacketInfo& info, PsiTable& table)
{
    uint8_t pointerOffset = info.payloadStartOffset;

    const uint8_t pointer_field = packet[pointerOffset];
    pointerOffset += sizeof(pointer_field);
    pointerOffset += pointer_field;

    resetBits(packet, TS_PACKET_SIZE, pointerOffset);

    table.table_id = getBits(8);
    table.section_syntax_indicator = getBits(1);
    getBits(1); // '0'
    getBits(2); // reserved
    table.section_length = getBits(12);
    table.transport_stream_id = getBits(16);
    getBits(2);
    table.version_number = getBits(5);
    table.current_next_indicator = getBits(1);
    table.section_number = getBits(8);
    table.last_section_number = getBits(8);
}


PatTable TsParser::parsePatPacket(const uint8_t* packet, const TsPacketInfo& info)
{
    PatTable pat;
    parsePsiTable(packet, info, pat);

    int numberOfPrograms = (pat.section_length - PAT_PACKET_OFFSET_LENGTH - CRC32_SIZE) / PAT_PACKET_PROGRAM_SIZE;

    for (int i = 0; i < numberOfPrograms; i++)
    {
        Program prg;
        prg.program_number = getBits(16);
        getBits(3); // reserved
        prg.program_map_PID = getBits(13);
        pat.programs.push_back(prg);
    }

    return pat;
}

// TODO support PMTs greater than 1 Ts packet.
PmtTable TsParser::parsePmtPacket(const uint8_t* packet, const TsPacketInfo& info)
{
    PmtTable pmt;
    parsePsiTable(packet, info, pmt);

    getBits(3); // reserved
    pmt.PCR_PID = getBits(13);
    getBits(4); // reserved
    pmt.program_info_length = getBits(12);
    getBits(8 * pmt.program_info_length); // skip descriptors for now

    int streamsSize = (pmt.section_length - PMT_PACKET_OFFSET_LENGTH - CRC32_SIZE - pmt.program_info_length);

    int readSize = 0;
    while(readSize < streamsSize)
    {
        StreamTypeHeader hdr;
        hdr.stream_type = getBits(8);
        getBits(3); // reserved
        hdr.elementary_PID = getBits(13);
        getBits(4); // reserved
        hdr.ES_info_length = getBits(12);
        getBits(hdr.ES_info_length * 8); // Skip for now
        readSize += hdr.ES_info_length + PMT_STREAM_TYPE_LENGTH;
        pmt.streams.push_back(hdr);
    }

    return pmt;
}
