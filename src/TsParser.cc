/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */

/// 3rd-party
#include <plog/Log.h>

/// Project files
#include "Logging.h"
#include "TsParser.h"

namespace mpeg2ts
{

void TsParser::parseTsPacketInfo(const uint8_t* a_packet, TsPacketInfo& a_outInfo)
{
    TsPacketInfo zero;
    a_outInfo = zero;

    TsHeader hdr = parseTsHeader(a_packet);
    a_outInfo.pid = hdr.PID;
    a_outInfo.hdr = hdr;
    a_outInfo.errorIndicator = hdr.transport_error_indicator;
    a_outInfo.isPayloadStart = hdr.payload_unit_start_indicator;
    a_outInfo.hasAdaptationField = checkHasAdaptationField(hdr);
    a_outInfo.hasPayload = checkHasPayload(hdr);
    a_outInfo.continuityCounter = hdr.continuity_counter;

    if (a_outInfo.hasAdaptationField)
    {
        parseAdaptationFieldData(a_packet, a_outInfo);

        if (a_outInfo.privateDataSize > 0)
        {
            a_outInfo.hasPrivateData = true;
        }
        else
        {
            a_outInfo.hasPrivateData = false;
        }
    }
    if (a_outInfo.hasPayload)
    {
        a_outInfo.payloadSize = TS_PACKET_SIZE - getByteInx();
        a_outInfo.payloadStartOffset = static_cast<uint8_t>(getByteInx());
    }
}


bool TsParser::checkSyncByte(const uint8_t* a_byte)
{
    return TS_PACKET_SYNC_BYTE == a_byte[0];
}


TsHeader TsParser::parseTsHeader(const uint8_t* a_packet)
{
    TsHeader hdr;
    resetBits(a_packet, TS_PACKET_SIZE);

    hdr.sync_byte = static_cast<uint8_t>(getBits(8));
    hdr.transport_error_indicator = getBits(1);
    hdr.payload_unit_start_indicator = getBits(1);
    hdr.transport_priority = getBits(1);
    hdr.PID = static_cast<int>(getBits(13));
    hdr.transport_scrambling_control = static_cast<uint8_t>(getBits(2));
    hdr.adaptation_field_control = static_cast<uint8_t>(getBits(2));
    hdr.continuity_counter = static_cast<uint8_t>(getBits(4));
    return hdr;
}


bool TsParser::checkHasAdaptationField(TsHeader a_hdr)
{
    return (a_hdr.adaptation_field_control == TS_ADAPTATION_FIELD_CONTROL_ADAPTATION_ONLY ||
            a_hdr.adaptation_field_control == TS_ADAPTATION_FIELD_CONTROL_ADAPTATION_PAYLOAD);
}


bool TsParser::checkHasPayload(TsHeader a_hdr)
{
    return (a_hdr.adaptation_field_control == TS_ADAPTATION_FIELD_CONTROL_PAYLOAD_ONLY ||
            a_hdr.adaptation_field_control == TS_ADAPTATION_FIELD_CONTROL_ADAPTATION_PAYLOAD);
}


TsAdaptationFieldHeader TsParser::parseAdaptationFieldHeader()
{
    TsAdaptationFieldHeader hdr = {}; // zero initialization
    hdr.adaptation_field_length = static_cast<uint8_t>(getBits(8));
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
void TsParser::parseAdaptationFieldData(const uint8_t* a_packet, TsPacketInfo& a_outInfo)
{
    const TsAdaptationFieldHeader adaptHdr = parseAdaptationFieldHeader();
    // printf("AF len: %d\n", adaptHdr.adaptation_field_length);
    a_outInfo.pcr = -1;
    a_outInfo.opcr = -1;
    if (adaptHdr.adaptation_field_length == 0)
    {
        return;
    }

    auto ofsAfterAF =
    getByteInx() - 1 + adaptHdr.adaptation_field_length; //-1 8 flags in TsAdaptationFieldHeader

    if (adaptHdr.PCR_flag)
    {
        a_outInfo.pcr = parsePcr();
    }
    if (adaptHdr.OPCR_flag)
    {
        a_outInfo.opcr = parsePcr();
    }
    if (adaptHdr.splicing_point_flag)
    {
        /*outData.splice_countdown = */ getBits(8);
    }

    if (adaptHdr.transport_private_data_flag)
    {
        a_outInfo.privateDataSize = static_cast<uint32_t>(getBits(8));
        a_outInfo.privateDataOffset = static_cast<uint32_t>(getByteInx());

        // Check if data size is within boundary of a TS Packet
        if (a_outInfo.privateDataSize > (TS_PACKET_SIZE - a_outInfo.privateDataOffset))
        {
            LOGE_(FileLog) << "Found out of bound private data. Error in input.";
            a_outInfo.isError = true;
            return;
        }

        for (uint32_t i = 0; i < a_outInfo.privateDataSize; ++i) // skip it for now
        {
            getBits(8);
        }
    }

    if (adaptHdr.adaptation_field_extension_flag)
    {
        const uint8_t adaptation_field_extension_length = static_cast<uint8_t>(getBits(8));

        // Check if data size is within boundary of a TS Packet
        if (adaptation_field_extension_length > (TS_PACKET_SIZE - getByteInx()))
        {
            LOGE_(FileLog) << "Found out of bound adaptation field extension data. Error in input.";
            a_outInfo.isError = true;
            return;
        }

        for (uint8_t i = 0; i < adaptation_field_extension_length; ++i) // skip it for now
        {
            getBits(8);
        }
    }
    a_outInfo.isDiscontinuity = adaptHdr.discontinuity_indicator;

    // 0..N stuffing bytes goes here and we have to adjust read offset
    resetBits(a_packet, TS_PACKET_SIZE, ofsAfterAF);
}


uint64_t TsParser::parsePcr()
{
    uint64_t pcr_base = 0;
    uint64_t pcr_extension = 0;

    pcr_base = getBits(33);
    getBits(6); // reserved

    pcr_extension = getBits(9);
    pcr_base = pcr_base * 300;
    pcr_base += pcr_extension;

    return pcr_base;
}


void TsParser::collectTable(const uint8_t* a_tsPacket, const TsPacketInfo& a_tsPacketInfo, int& a_table_id)
{
    // NOTE!! this is not as easy as one might think. There maybe be alternating PSI long tables and
    // it has been confirmed by assets that some long PMT can be mixed with PAT tables in between.
    // Therefore we need be able collect different types of tables on their PID to handle this. If
    // we don't do it, the alternating table will reset the previous collected table since it start
    // over all from the beginning.
    int PID =
    a_tsPacketInfo.pid; // There is a good reason, please see above to have a filter on PID...
    uint16_t pointerOffset = a_tsPacketInfo.payloadStartOffset;

    mStatistics.checkCCError(a_tsPacketInfo.pid, a_tsPacketInfo.continuityCounter);
    mStatistics.checkTsDiscontinuity(a_tsPacketInfo.pid, a_tsPacketInfo.hasAdaptationField &&
                                                         a_tsPacketInfo.isDiscontinuity);

    if (a_tsPacketInfo.hdr.payload_unit_start_indicator)
    {
        mSectionBuffer[PID].clear();
        mReadSectionLength[PID] = 0;

        uint16_t pointer_field = a_tsPacket[pointerOffset];

        pointerOffset++; // pointerOffset += sizeof(pointer_field);
        pointerOffset = static_cast<uint16_t>(pointerOffset + pointer_field);
        //
        // It does only make sense to get PSI table info when start of a PSI.
        // Note the other payloads of PSI spans in more than 1 ts-packet will not contain
        // any PSI table info, just continuation of their content in the ts-packet payload...
        PsiTable tableInfo;
        ByteVector tmpBuffer;
        tmpBuffer.insert(tmpBuffer.begin(), &a_tsPacket[pointerOffset], &a_tsPacket[TS_PACKET_SIZE]);
        resetBits(tmpBuffer.data(), TS_PACKET_SIZE - pointerOffset, 0);
        parsePsiTable(tmpBuffer, tableInfo);

        // If PMT spans more than 1 ts-packet we need collect all of them...
        mSectionLength[PID] = tableInfo.section_length + 3;
        mTableId[PID] = tableInfo.table_id;
    }

    mSectionBuffer[PID].insert(mSectionBuffer[PID].end(), &a_tsPacket[pointerOffset], &a_tsPacket[TS_PACKET_SIZE]);
    mReadSectionLength[PID] =
    mReadSectionLength[PID] + static_cast<int>(&a_tsPacket[TS_PACKET_SIZE] - &a_tsPacket[pointerOffset]);
    a_table_id = (mSectionLength[PID] > mReadSectionLength[PID]) ? PSI_TABLE_ID_INCOMPLETE : mTableId[PID];
}


bool TsParser::collectPes(const uint8_t* a_tsPacket, const TsPacketInfo& a_tsPacketInfo, PesPacket& a_pesPacket)
{
    bool ret = false;
    const uint8_t pointerOffset = a_tsPacketInfo.payloadStartOffset;
    auto pid = a_tsPacketInfo.pid;

    mStatistics.checkCCError(pid, a_tsPacketInfo.continuityCounter);
    mStatistics.checkTsDiscontinuity(pid, a_tsPacketInfo.hasAdaptationField && a_tsPacketInfo.isDiscontinuity);
    if (a_tsPacketInfo.hasAdaptationField)
    {
        mStatistics.buildPcrHistogram(pid, a_tsPacketInfo.pcr);
    }

    if (a_tsPacketInfo.isPayloadStart)
    {
        // We have start. So if we have any cached data it's time to return it.
        if (!mPesPacket[pid].mPesBuffer.empty())
        {
            if (mPesPacket[pid].PES_packet_length &&
                mPesPacket[pid].mPesBuffer.size() < mPesPacket[pid].PES_packet_length)
            {
                std::cerr << "Not returning incomplete PES packet on pid " << pid << '\n';
            }
            else
            {
                a_pesPacket = mPesPacket[pid]; // TODO: must copy as we override it below.

                mStatistics.buildPtsHistogram(pid, a_pesPacket.pts);
                mStatistics.buildDtsHistogram(pid, a_pesPacket.dts);

                ret = true;
            }
        }

        // Create new PES
        mPesPacket[pid] = PesPacket();
        pid = a_tsPacketInfo.pid;

        mPesPacket[pid].mPesBuffer.insert(mPesPacket[pid].mPesBuffer.end(),
                                          &a_tsPacket[pointerOffset], &a_tsPacket[TS_PACKET_SIZE]);

        parsePesPacket(pid);
    }
    else
    {
        if (mPesPacket.count(pid) == 0)
        {
            // PES has not started yet. Ignoring rest
            return false;
        }

        // Assemble packet
        mPesPacket[pid].mPesBuffer.insert(mPesPacket[pid].mPesBuffer.end(),
                                          &a_tsPacket[pointerOffset], &a_tsPacket[TS_PACKET_SIZE]);
        // TODO: check if we have boud PES and return it if it is coplete
    }

    return ret;
}

void TsParser::parsePsiTable(const ByteVector& /* table*/, PsiTable& a_tableInfo)
{
    a_tableInfo.table_id = static_cast<uint8_t>(getBits(8));
    a_tableInfo.section_syntax_indicator = getBits(1);
    getBits(1); // '0'
    getBits(2); // reserved
    a_tableInfo.section_length = static_cast<uint16_t>(getBits(12));
    a_tableInfo.transport_stream_id = static_cast<uint16_t>(getBits(16));
    getBits(2); // reserved
    a_tableInfo.version_number = static_cast<uint8_t>(getBits(5));
    a_tableInfo.current_next_indicator = getBits(1);
    a_tableInfo.section_number = static_cast<uint8_t>(getBits(8));
    a_tableInfo.last_section_number = static_cast<uint8_t>(getBits(8));
}


PatTable TsParser::parsePatPacket(int a_pid)
{
    PatTable pat;
    resetBits(mSectionBuffer[a_pid].data(), mSectionBuffer[a_pid].size(), 0);
    parsePsiTable(mSectionBuffer[a_pid], pat);

    const int numberOfPrograms =
    (pat.section_length - PAT_PACKET_OFFSET_LENGTH - CRC32_SIZE) / PAT_PACKET_PROGRAM_SIZE;

    for (int i = 0; i < numberOfPrograms; ++i)
    {
        Program prg;
        prg.program_number = static_cast<uint16_t>(getBits(16));
        getBits(3); // reserved
        uint16_t PID = static_cast<uint16_t>(getBits(13));

        if (prg.program_number == 0)
        {
            prg.type = ProgramType::NIT;
            prg.network_PID = PID;
        }
        else
        {
            prg.type = ProgramType::PMT;
            prg.program_map_PID = PID;
        }

        pat.programs.push_back(std::move(prg));
    }

    return pat;
}


PmtTable TsParser::parsePmtPacket(int a_pid)
{
    PmtTable pmt;
    resetBits(mSectionBuffer[a_pid].data(), mSectionBuffer[a_pid].size(), 0);
    parsePsiTable(mSectionBuffer[a_pid], pmt);

    getBits(3); // reserved
    pmt.PCR_PID = static_cast<uint16_t>(getBits(13));
    getBits(4); // reserved
    pmt.program_info_length = static_cast<uint16_t>(getBits(12));
    uint32_t control_bits = pmt.program_info_length & 0xC00;
    if ((control_bits >> 10) != 0)
    {
        LOGE_(FileLog)
        << "Stream not following ISO/IEC 13818-1 in program_info_length control bits != 0.";
    }
    int program_info_length = pmt.program_info_length & 0x3FF;


    if (program_info_length != 0)
    {
        // TODO function parseDescriptors...
        uint8_t descriptorTag = static_cast<uint8_t>(getBits(8));

        LOGV << "descriptor_tag: " << static_cast<int>(descriptorTag);
        DescriptorTag tag = static_cast<DescriptorTag>(descriptorTag);
        switch (tag)
        {
        case DescriptorTag::maximum_bitrate_descriptor:
        {
            MaximumBitrateDescriptor maxDesc;
            maxDesc.descriptor_tag = descriptorTag;
            maxDesc.descriptor_length = static_cast<uint8_t>(getBits(8));

            maxDesc.reserved = static_cast<uint8_t>(getBits(2));
            maxDesc.maximum_bitrate = static_cast<uint32_t>(getBits(22));
            pmt.descriptors.push_back(std::move(maxDesc));
            skipBytes(program_info_length - 2 - 3);
            break;
        }
        case DescriptorTag::metadata_pointer_descriptor:
        {
            Metadata_pointer_descriptor pointer_desc;
            pointer_desc.descriptor_tag = descriptorTag;
            pointer_desc.descriptor_length = static_cast<uint8_t>(getBits(8));

            pointer_desc.metadata_application_format = static_cast<uint16_t>(getBits(16));

            pmt.descriptors.push_back(std::move(pointer_desc));
            skipBytes(program_info_length - 2 -
                      2); // TODO fix this, this is a much bigger descriptor...
            break;
        }
        case DescriptorTag::user_private_178:
        {
            Metadata_pointer_descriptor user_private;
            user_private.descriptor_tag = descriptorTag;
            user_private.descriptor_length = static_cast<uint8_t>(getBits(8));

            pmt.descriptors.push_back(std::move(user_private));
            skipBytes(program_info_length -
                      2); // TODO fix this, this is a much bigger descriptor...
            break;
        }
        default:
            LOGV << "skipping descriptor.." << '\n';
            skipBytes(program_info_length - 1); // skip descriptors for now
        }
    }


    int streamsSize = (pmt.section_length - PMT_PACKET_OFFSET_LENGTH - CRC32_SIZE - pmt.program_info_length);
    int readSize{ 0 };

    while (readSize < streamsSize)
    {
        StreamTypeHeader hdr;
        hdr.stream_type = static_cast<StreamType>(getBits(8));
        getBits(3); // reserved
        hdr.elementary_PID = static_cast<uint16_t>(getBits(13));
        getBits(4); // reserved
        hdr.ES_info_length = static_cast<uint16_t>(getBits(12));
        control_bits = hdr.ES_info_length & 0xC00;
        if ((control_bits >> 10) != 0)
        {
            LOGE_(FileLog)
            << "Stream not following ISO/IEC 13818-1 in ES_info_length control bits != 0.";
        }
        uint32_t ES_info_length = hdr.ES_info_length & 0x3FF;
        skipBytes(ES_info_length);
        readSize += (ES_info_length + PMT_STREAM_TYPE_LENGTH);
        pmt.streams.push_back(std::move(hdr));
    }

    return pmt;
}


void TsParser::parsePesPacket(int a_pid)
{
    resetBits(mPesPacket[a_pid].mPesBuffer.data(), TS_PACKET_SIZE, 0);

    mPesPacket[a_pid].packet_start_code_prefix = static_cast<uint32_t>(getBits(24));
    mPesPacket[a_pid].stream_id = static_cast<uint8_t>(getBits(8));
    mPesPacket[a_pid].PES_packet_length = static_cast<uint16_t>(getBits(16));
    mPesPacket[a_pid].elementary_data_offset = 0;

    // ISO/IEC 13818-1:2015: Table 2-21 PES packet
    if (mPesPacket[a_pid].stream_id != STREAM_ID_program_stream_map &&
        mPesPacket[a_pid].stream_id != STREAM_ID_padding_stream &&
        mPesPacket[a_pid].stream_id != STREAM_ID_private_stream_2 &&
        mPesPacket[a_pid].stream_id != STREAM_ID_ECM_stream && mPesPacket[a_pid].stream_id != STREAM_ID_EMM_stream &&
        mPesPacket[a_pid].stream_id != STREAM_ID_program_stream_directory &&
        mPesPacket[a_pid].stream_id != STREAM_ID_DSMCC_stream &&
        mPesPacket[a_pid].stream_id != STREAM_ID_ITU_T_Rec_H222_1_type_E_stream)
    {
        getBits(2); // '10'
        mPesPacket[a_pid].PES_scrambling_control = getBits(2);

        mPesPacket[a_pid].PES_priority = getBits(1);
        mPesPacket[a_pid].data_alignment_indicator = getBits(1);
        mPesPacket[a_pid].copyright = getBits(1);
        mPesPacket[a_pid].original_or_copy = getBits(1);
        mPesPacket[a_pid].PTS_DTS_flags = static_cast<uint8_t>(getBits(2));
        mPesPacket[a_pid].ESCR_flag = getBits(1);
        mPesPacket[a_pid].ES_rate_flag = getBits(1);
        mPesPacket[a_pid].DSM_trick_mode_flag = getBits(1);
        mPesPacket[a_pid].additional_copy_info_flag = getBits(1);
        mPesPacket[a_pid].PES_CRC_flag = getBits(1);
        mPesPacket[a_pid].PES_extension_flag = getBits(1);

        mPesPacket[a_pid].PES_header_data_length = static_cast<uint8_t>(getBits(8));
        mPesPacket[a_pid].elementary_data_offset =
        static_cast<uint16_t>(mPesPacket[a_pid].PES_header_data_length + getByteInx());

        mPesPacket[a_pid].pts = -1;
        mPesPacket[a_pid].dts = -1;
        // Forbidden value
        if (mPesPacket[a_pid].PTS_DTS_flags == 0x01)
        {
            LOGE_(FileLog) << "Forbidden PTS_DTS_flags:" << mPesPacket[a_pid].PTS_DTS_flags;
        }
        else if (mPesPacket[a_pid].PTS_DTS_flags == 0x02) // Only PTS value
        {
            getBits(4);
            uint64_t pts = 0;
            uint64_t pts_32_30 = getBits(3);
            getBits(1); // marker_bit
            uint64_t pts_29_15 = getBits(15);
            getBits(1); // marker_bit
            uint64_t pts_14_0 = getBits(15);
            getBits(1); // marker_bit

            pts = (pts_32_30 << 30) + (pts_29_15 << 15) + pts_14_0;

            mPesPacket[a_pid].pts = pts;
            mPesPacket[a_pid].dts = -1;
        }
        else if (mPesPacket[a_pid].PTS_DTS_flags == 0x03) // Both PTS and DTS
        {
            getBits(4);
            uint64_t pts = 0;
            uint64_t pts_32_30 = getBits(3);
            getBits(1); // marker_bit
            uint64_t pts_29_15 = getBits(15);
            getBits(1); // marker_bit
            uint64_t pts_14_0 = getBits(15);
            getBits(1); // marker_bit

            pts = (pts_32_30 << 30) + (pts_29_15 << 15) + pts_14_0;

            mPesPacket[a_pid].pts = pts;

            getBits(4);
            uint64_t dts = 0;
            uint64_t dts_32_30 = getBits(3);
            getBits(1); // marker_bit
            uint64_t dts_29_15 = getBits(15);
            getBits(1); // marker_bit
            uint64_t dts_14_0 = getBits(15);
            getBits(1); // marker_bit

            dts = (dts_32_30 << 30) + (dts_29_15 << 15) + dts_14_0;

            mPesPacket[a_pid].dts = dts;
        }
    }
}

ByteVector& TsParser::getRawTable(int a_pid)
{
    return mSectionBuffer[a_pid];
}

} // namespace mpeg2ts
