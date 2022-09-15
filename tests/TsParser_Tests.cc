/*****************************************************************
*
*    Copyright © 2017-2022 kohnech, lnwhome All rights reserved
*
*    mpeg2ts - mpeg2ts tests
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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

/// Project files
#include "TsPacketTestData.h"
#include "TsParser.h"
#include "mpeg2ts.h"

using namespace mpeg2ts;

TEST(TsParserConstants, CheckConstants)
{
    EXPECT_EQ(TS_PACKET_HEADER_SIZE, 4);
    EXPECT_EQ(TS_PACKET_SIZE, 188);
    EXPECT_EQ(TS_PACKET_SYNC_BYTE, 0x47);
    EXPECT_EQ(TS_PACKET_MAX_PAYLOAD_SIZE, 184);
    EXPECT_EQ(TS_PACKET_PID_PAT, 0);
    EXPECT_EQ(TS_PACKET_PID_NULL, 0x1fff);
    EXPECT_EQ(PCR_SIZE, 6);
    EXPECT_EQ(OPCR_SIZE, 6);
}

TEST(TsParserConstants, CheckSyncByte)
{
    TsParser parser;
    const uint8_t packet_data[TS_PACKET_SIZE] = { 0x47 };
    EXPECT_TRUE(parser.checkSyncByte(packet_data));
    EXPECT_TRUE(parser.checkSyncByte(packet_1));
    EXPECT_TRUE(parser.checkSyncByte(packet_2));
    EXPECT_TRUE(parser.checkSyncByte(packet_3));
}

TEST(TsParserTests, CheckParserInfo)
{
    TsParser parser;
    // TsHeader hdr = parser.parseTsHeader(packet_1);

    TsPacketInfo info;
    parser.parseTsPacketInfo(packet_1, info);

    EXPECT_EQ(-1, info.pcr);
    EXPECT_EQ(-1, info.opcr);
    EXPECT_EQ(47, info.payloadSize);
    EXPECT_EQ(141, info.payloadStartOffset);
    EXPECT_TRUE(info.hasAdaptationField);

    EXPECT_EQ(0x47, info.hdr.sync_byte);
    EXPECT_EQ(3, info.hdr.adaptation_field_control);
    EXPECT_FALSE(info.isPayloadStart);
    EXPECT_TRUE(info.hasAdaptationField);
    EXPECT_TRUE(info.hasPayload);
    EXPECT_FALSE(info.hasPrivateData);
    EXPECT_FALSE(info.isScrambled);
    EXPECT_FALSE(info.isDiscontinuity);
    EXPECT_FALSE(info.continuityCounter);
    EXPECT_EQ(-1, info.pcr);
    EXPECT_EQ(-1, info.opcr);
    EXPECT_EQ(-1, info.pts);
    EXPECT_EQ(-1, info.dts);
    EXPECT_EQ(47, info.payloadSize);
    EXPECT_EQ(141, info.payloadStartOffset);
    EXPECT_FALSE(info.isError);
}

TEST(TsParserTests, CheckPid)
{
    TsParser parser;
    // TsHeader hdr = parser.parseTsHeader(packet_1);

    TsPacketInfo info;
    parser.parseTsPacketInfo(packet_3, info);
    EXPECT_EQ(289, info.pid);
    EXPECT_EQ(289, info.hdr.PID);
    parser.parseTsPacketInfo(packet_4, info);
    EXPECT_EQ(481, info.pid);
    // TODO add more tests...
}

TEST(TsParserTests, CheckParsePatTable)
{
    TsParser parser;
    // TsHeader hdr = parser.parseTsHeader(packet_1);

    PatTable pat;
    TsPacketInfo info;
    parser.parseTsPacketInfo(pat_packet_1, info);
    int table_id;
    EXPECT_EQ(info.pid, 0);
    parser.collectTable(pat_packet_1, info, table_id);
    EXPECT_EQ(PSI_TABLE_ID_PAT, table_id);
    pat = parser.parsePatPacket(info.pid);
    EXPECT_EQ(TS_PACKET_PID_PAT, info.pid) << "Wrong PID for PAT, should be 0!";
    EXPECT_EQ(PSI_TABLE_ID_PAT, pat.table_id) << "Wrong Table id for PAT";
    EXPECT_EQ(pat.programs.size(), 1) << "Got wrong numbers of PMTs in PAT!";
    EXPECT_EQ(pat.programs.at(0).program_map_PID, 598) << "Got wrong PMT PID!";
}

TEST(TsParserTests, CheckParsePatTable2)
{
    TsParser parser;
    PatTable pat;
    TsPacketInfo info;

    parser.parseTsPacketInfo(pat_packet_2, info);
    int table_id;
    parser.collectTable(pat_packet_2, info, table_id);
    EXPECT_EQ(PSI_TABLE_ID_PAT, table_id);
    pat = parser.parsePatPacket(info.pid);
    EXPECT_EQ(TS_PACKET_PID_PAT, info.pid);
    EXPECT_EQ(PSI_TABLE_ID_PAT, pat.table_id);

    EXPECT_EQ(0x04d, pat.section_length);
    EXPECT_EQ(0x03fd, pat.transport_stream_id);
    EXPECT_EQ(1, pat.version_number);
    EXPECT_EQ(0, pat.section_number);
    EXPECT_EQ(0, pat.last_section_number);

    EXPECT_EQ(17, pat.programs.size());

    EXPECT_EQ(0, pat.programs[0].program_number);
    EXPECT_EQ(0x10, pat.programs[0].program_map_PID);

    EXPECT_EQ(0x143c, pat.programs[1].program_number);
    EXPECT_EQ(0x143c, pat.programs[1].program_map_PID);

    EXPECT_EQ(0x13ba, pat.programs[2].program_number);
    EXPECT_EQ(0x13ba, pat.programs[2].program_map_PID);

    EXPECT_EQ(0x13ce, pat.programs[3].program_number);
    EXPECT_EQ(0x13ce, pat.programs[3].program_map_PID);

    EXPECT_EQ(0x15a4, pat.programs[4].program_number);
    EXPECT_EQ(0x15a4, pat.programs[4].program_map_PID);

    EXPECT_EQ(0x4d8, pat.programs[5].program_number);
    EXPECT_EQ(0x4d8, pat.programs[5].program_map_PID);

    EXPECT_EQ(0x1608, pat.programs[6].program_number);
    EXPECT_EQ(0x1608, pat.programs[6].program_map_PID);

    EXPECT_EQ(0x3f2, pat.programs[7].program_number);
    EXPECT_EQ(0x3f2, pat.programs[7].program_map_PID);

    EXPECT_EQ(0x16d0, pat.programs[8].program_number);
    EXPECT_EQ(0x16d0, pat.programs[8].program_map_PID);

    EXPECT_EQ(0x16a8, pat.programs[9].program_number);
    EXPECT_EQ(0x16a8, pat.programs[9].program_map_PID);

    EXPECT_EQ(0x50a, pat.programs[10].program_number);
    EXPECT_EQ(0x50a, pat.programs[10].program_map_PID);

    EXPECT_EQ(0x500, pat.programs[11].program_number);
    EXPECT_EQ(0x500, pat.programs[11].program_map_PID);

    EXPECT_EQ(0x366, pat.programs[12].program_number);
    EXPECT_EQ(0x366, pat.programs[12].program_map_PID);

    EXPECT_EQ(0xfffe, pat.programs[13].program_number);
    EXPECT_EQ(0x712, pat.programs[13].program_map_PID);

    EXPECT_EQ(0xbc2, pat.programs[14].program_number);
    EXPECT_EQ(0xbc2, pat.programs[14].program_map_PID);

    EXPECT_EQ(0xbcc, pat.programs[15].program_number);
    EXPECT_EQ(0xbcc, pat.programs[15].program_map_PID);

    EXPECT_EQ(0xbd6, pat.programs[16].program_number);
    EXPECT_EQ(0xbd6, pat.programs[16].program_map_PID);
}

/*!
 * Parses a normal PMT table
 */
TEST(TsParserTests, CheckParsePmtTable)
{
    TsParser parser;
    PmtTable pmt;
    TsPacketInfo info;

    parser.parseTsPacketInfo(pmt_packet_1, info);
    int table_id;
    parser.collectTable(pmt_packet_1, info, table_id);
    EXPECT_EQ(PSI_TABLE_ID_PMT, table_id);
    pmt = parser.parsePmtPacket(info.pid);
    EXPECT_EQ(1010, info.pid);
    EXPECT_EQ(PSI_TABLE_ID_PMT, pmt.table_id);

    EXPECT_EQ(1, pmt.section_syntax_indicator);
    EXPECT_EQ(0x034, pmt.section_length);
    EXPECT_EQ(0x03f2, pmt.transport_stream_id);
    EXPECT_EQ((0xed >> 1) & 0x1F, pmt.version_number);
    EXPECT_EQ(1, pmt.current_next_indicator);
    EXPECT_EQ(0x00, pmt.section_number);
    EXPECT_EQ(0, pmt.last_section_number);

    // Extensions from PsiTable
    EXPECT_EQ(0x03fb, pmt.PCR_PID);
    EXPECT_EQ(0, pmt.program_info_length);

    EXPECT_EQ(4, pmt.streams.size());
    // ESPID=1004(0x3ec)
    EXPECT_EQ(0x6, pmt.streams[0].stream_type);
    EXPECT_EQ(0x3ec, pmt.streams[0].elementary_PID);
    EXPECT_EQ(7, pmt.streams[0].ES_info_length);

    // ESPID=1018(0x3fa)
    EXPECT_EQ(0x3, pmt.streams[1].stream_type);
    EXPECT_EQ(0x3fa, pmt.streams[1].elementary_PID);
    EXPECT_EQ(0, pmt.streams[1].ES_info_length);

    // ESPID=1017(0x3f9)
    EXPECT_EQ(0x6, pmt.streams[2].stream_type);
    EXPECT_EQ(0x3f9, pmt.streams[2].elementary_PID);
    EXPECT_EQ(12, pmt.streams[2].ES_info_length);

    // ESPID=1019(0x3fb)
    EXPECT_EQ(0x2, pmt.streams[3].stream_type);
    EXPECT_EQ(0x3fb, pmt.streams[3].elementary_PID);
    EXPECT_EQ(0, pmt.streams[3].ES_info_length);
}

TEST(TsParserTests, CheckParseTsHeader)
{
    TsParser parser;
    TsHeader hdr = parser.parseTsHeader(packet_3);

    EXPECT_EQ(0x47, hdr.sync_byte);
    EXPECT_FALSE(hdr.transport_error_indicator);
    EXPECT_TRUE(hdr.payload_unit_start_indicator);
    EXPECT_FALSE(hdr.transport_priority);
    EXPECT_EQ(289, hdr.PID);
    EXPECT_EQ(0, hdr.transport_scrambling_control);
    EXPECT_EQ(TS_ADAPTATION_FIELD_CONTROL_PAYLOAD_ONLY, hdr.adaptation_field_control);
    EXPECT_EQ(0xc, hdr.continuity_counter);
}

/*!
 * Parses a normal PMT table
 */
TEST(TsParserTests, CheckParsePmtTable2)
{
    TsParser parser;

    TsPacketInfo info;
    parser.parseTsPacketInfo(pmt_packet_2_1, info);
    int table_id;
    parser.collectTable(pmt_packet_2_1, info, table_id);
    EXPECT_EQ(PSI_TABLE_ID_INCOMPLETE, table_id);
    parser.parseTsPacketInfo(pmt_packet_2_2, info);
    parser.collectTable(pmt_packet_2_2, info, table_id);
    EXPECT_EQ(PSI_TABLE_ID_PMT, table_id);

    auto pmt = parser.parsePmtPacket(info.pid);
    EXPECT_EQ(32, info.pid);
    EXPECT_EQ(PSI_TABLE_ID_PMT, pmt.table_id);

    // Extensions from PsiTable
    EXPECT_EQ(599, pmt.PCR_PID);
    EXPECT_EQ(22, pmt.program_info_length);

    EXPECT_EQ(9, pmt.streams.size());

    EXPECT_EQ(21, pmt.streams[8].stream_type);
    EXPECT_EQ(144, pmt.streams[8].elementary_PID);
    EXPECT_EQ(26, pmt.streams[8].ES_info_length);
}

/*!
 * Parses a large PMT table across 3 ts-packets
 */
TEST(TsParserTests, CheckParseLargePmtTable)
{
    try
    {
        TsParser parser;

        TsPacketInfo info;
        int table_id;
        parser.parseTsPacketInfo(large_pmt_ts_packet_1, info);
        EXPECT_EQ(50, info.pid);
        parser.collectTable(large_pmt_ts_packet_1, info, table_id);
        EXPECT_EQ(PSI_TABLE_ID_INCOMPLETE, table_id);

        parser.parseTsPacketInfo(large_pmt_ts_packet_2, info);
        parser.collectTable(large_pmt_ts_packet_2, info, table_id);
        EXPECT_EQ(PSI_TABLE_ID_INCOMPLETE, table_id);

        parser.parseTsPacketInfo(large_pmt_ts_packet_2, info);
        parser.collectTable(large_pmt_ts_packet_3, info, table_id);
        EXPECT_EQ(PSI_TABLE_ID_PMT, table_id);

        auto pmt = parser.parsePmtPacket(info.pid);
        EXPECT_EQ(50, info.pid);
        EXPECT_EQ(PSI_TABLE_ID_PMT, pmt.table_id);
        // Extensions from PsiTable
        EXPECT_EQ(110, pmt.PCR_PID);
        EXPECT_EQ(104, pmt.program_info_length);
        EXPECT_EQ(5, pmt.streams.size());

        EXPECT_EQ(STREAMTYPE_VIDEO_H264, pmt.streams[0].stream_type);
        EXPECT_EQ(110, pmt.streams[0].elementary_PID);
        EXPECT_EQ(97, pmt.streams[0].ES_info_length);

        EXPECT_EQ(STREAMTYPE_AUDIO_MPEG2, pmt.streams[1].stream_type);
        EXPECT_EQ(210, pmt.streams[1].elementary_PID);
        EXPECT_EQ(92, pmt.streams[1].ES_info_length);

        EXPECT_EQ(STREAMTYPE_PRIVATE_PES, pmt.streams[2].stream_type);
        EXPECT_EQ(310, pmt.streams[2].elementary_PID);
        EXPECT_EQ(107, pmt.streams[2].ES_info_length);

        EXPECT_EQ(STREAMTYPE_PRIVATE_PES, pmt.streams[3].stream_type);
        EXPECT_EQ(1410, pmt.streams[3].elementary_PID);
        EXPECT_EQ(10, pmt.streams[3].ES_info_length);

        EXPECT_EQ(STREAMTYPE_PRIVATE_PES, pmt.streams[4].stream_type);
        EXPECT_EQ(1310, pmt.streams[4].elementary_PID);
        EXPECT_EQ(20, pmt.streams[4].ES_info_length);
    }
    catch (std::exception& e)
    {
        FAIL() << "Should not catch exception";
    }
}


/*
* Parses a complete PES packet distributed on many TS packets.
*/
TEST(TsParserTests, TestParsePesPacket)
{
    TsParser parser;
    PesPacket pes;
    TsPacketInfo info;

    parser.parseTsPacketInfo(ts_pes_1, info);
    EXPECT_EQ(50, info.pid);
    EXPECT_FALSE(parser.collectPes(ts_pes_1, info, pes));

    parser.parseTsPacketInfo(ts_pes_2, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_2, info, pes));

    parser.parseTsPacketInfo(ts_pes_3, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_3, info, pes));

    parser.parseTsPacketInfo(ts_pes_4, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_4, info, pes));

    parser.parseTsPacketInfo(ts_pes_5, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_5, info, pes));

    parser.parseTsPacketInfo(ts_pes_6, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_6, info, pes));

    parser.parseTsPacketInfo(ts_pes_7, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_7, info, pes));

    parser.parseTsPacketInfo(ts_pes_8, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_8, info, pes));

    parser.parseTsPacketInfo(ts_pes_9, info);
    EXPECT_TRUE(parser.collectPes(ts_pes_9, info, pes));
    
    EXPECT_EQ(PES_PACKET_START_CODE_PREFIX, pes.packet_start_code_prefix);
    EXPECT_EQ(STREAM_ID_private_stream_1, pes.stream_id);
    EXPECT_EQ(1554, pes.PES_packet_length);
    EXPECT_TRUE(pes.PES_has_optional_header);
    EXPECT_FALSE(pes.PES_scrambling_control); // Unscrambled.
    EXPECT_FALSE(pes.PES_priority);
    EXPECT_TRUE(pes.data_alignment_indicator);
    EXPECT_FALSE(pes.copyright);
    EXPECT_FALSE(pes.original_or_copy);
    EXPECT_EQ(0x02, pes.PTS_DTS_flags); // only PTS flag (no DTS)
    EXPECT_FALSE(pes.ESCR_flag);

    EXPECT_FALSE(pes.ES_rate_flag);
    EXPECT_FALSE(pes.DSM_trick_mode_flag);
    EXPECT_FALSE(pes.additional_copy_info_flag);
    EXPECT_FALSE(pes.PES_CRC_flag);
    EXPECT_FALSE(pes.PES_extension_flag);
    EXPECT_EQ(15, pes.PES_header_data_length);
    EXPECT_EQ(161697, pes.pts);
    EXPECT_EQ(-1, pes.dts); // No DTS...    
}

/*
* Parses a complete PES packet distributed on many TS packets.
*/
TEST(TsParserTests, TestParsePesPacketLarge)
{
    TsParser parser;
    PesPacket pes;
    TsPacketInfo info;

    parser.parseTsPacketInfo(ts_pes_large1, info);
    EXPECT_EQ(210, info.pid);
    EXPECT_FALSE(parser.collectPes(ts_pes_large1, info, pes));

    parser.parseTsPacketInfo(ts_pes_large2, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large2, info, pes));

    parser.parseTsPacketInfo(ts_pes_large3, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large3, info, pes));

    parser.parseTsPacketInfo(ts_pes_large4, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large4, info, pes));

    parser.parseTsPacketInfo(ts_pes_large5, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large5, info, pes));

    parser.parseTsPacketInfo(ts_pes_large6, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large6, info, pes));

    parser.parseTsPacketInfo(ts_pes_large7, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large7, info, pes));

    parser.parseTsPacketInfo(ts_pes_large8, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large8, info, pes));

    parser.parseTsPacketInfo(ts_pes_large9, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large9, info, pes));

    parser.parseTsPacketInfo(ts_pes_large10, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large10, info, pes));

    parser.parseTsPacketInfo(ts_pes_large11, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large11, info, pes));

    parser.parseTsPacketInfo(ts_pes_large12, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large12, info, pes));

    parser.parseTsPacketInfo(ts_pes_large13, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large13, info, pes));

    parser.parseTsPacketInfo(ts_pes_large14, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large14, info, pes));

    parser.parseTsPacketInfo(ts_pes_large15, info);
    EXPECT_FALSE(parser.collectPes(ts_pes_large15, info, pes));

    parser.parseTsPacketInfo(ts_pes_large16, info);
    EXPECT_TRUE(parser.collectPes(ts_pes_large16, info, pes));

    EXPECT_EQ(PES_PACKET_START_CODE_PREFIX, pes.packet_start_code_prefix);
    EXPECT_EQ(STREAM_ID_pes_audio_stream, pes.stream_id);
    EXPECT_EQ(2890, pes.PES_packet_length);
    EXPECT_TRUE(pes.PES_has_optional_header);
    EXPECT_FALSE(pes.PES_scrambling_control); // Unscrambled.
    EXPECT_FALSE(pes.PES_priority);
    EXPECT_TRUE(pes.data_alignment_indicator);
    EXPECT_TRUE(pes.copyright);
    EXPECT_TRUE(pes.original_or_copy);
    EXPECT_EQ(0x02, pes.PTS_DTS_flags); // only PTS flag (no DTS)
    EXPECT_FALSE(pes.ESCR_flag);

    EXPECT_FALSE(pes.ES_rate_flag);
    EXPECT_FALSE(pes.DSM_trick_mode_flag);
    EXPECT_FALSE(pes.additional_copy_info_flag);
    EXPECT_FALSE(pes.PES_CRC_flag);
    EXPECT_FALSE(pes.PES_extension_flag);
    EXPECT_EQ(7, pes.PES_header_data_length);
    EXPECT_EQ(1426376378, pes.pts);
    EXPECT_EQ(-1, pes.dts); // No DTS...
}

/*
* Parses 2 complete PES packet distributed on many TS packets and 2 PIDs.
*/
TEST(TsParserTests, TestParseTwoPesPackets)
{
    TsParser parser;
    PesPacket pes50, pes210;
    TsPacketInfo info50, info210;

    parser.parseTsPacketInfo(ts_pes_large1, info210);
    EXPECT_EQ(210, info210.pid);
    EXPECT_FALSE(parser.collectPes(ts_pes_large1, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_1, info50);
    EXPECT_EQ(50, info50.pid);
    EXPECT_FALSE(parser.collectPes(ts_pes_1, info50, pes50));

    parser.parseTsPacketInfo(ts_pes_large2, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large2, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_2, info50);
    EXPECT_FALSE(parser.collectPes(ts_pes_2, info50, pes50));

    parser.parseTsPacketInfo(ts_pes_large3, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large3, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_3, info50);
    EXPECT_FALSE(parser.collectPes(ts_pes_3, info50, pes50));

    parser.parseTsPacketInfo(ts_pes_large4, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large4, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_4, info50);
    EXPECT_FALSE(parser.collectPes(ts_pes_4, info50, pes50));

    parser.parseTsPacketInfo(ts_pes_large5, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large5, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_5, info50);
    EXPECT_FALSE(parser.collectPes(ts_pes_5, info50, pes50));

    parser.parseTsPacketInfo(ts_pes_large6, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large6, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_6, info50);
    EXPECT_FALSE(parser.collectPes(ts_pes_6, info50, pes50));

    parser.parseTsPacketInfo(ts_pes_large7, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large7, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_7, info50);
    EXPECT_FALSE(parser.collectPes(ts_pes_7, info50, pes50));

    parser.parseTsPacketInfo(ts_pes_large8, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large8, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_8, info50);
    EXPECT_FALSE(parser.collectPes(ts_pes_8, info50, pes50));

    parser.parseTsPacketInfo(ts_pes_large9, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large9, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_9, info50);
    EXPECT_TRUE(parser.collectPes(ts_pes_9, info50, pes50));

    parser.parseTsPacketInfo(ts_pes_large10, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large10, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_large11, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large11, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_large12, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large12, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_large13, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large13, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_large14, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large14, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_large15, info210);
    EXPECT_FALSE(parser.collectPes(ts_pes_large15, info210, pes210));

    parser.parseTsPacketInfo(ts_pes_large16, info210);
    EXPECT_TRUE(parser.collectPes(ts_pes_large16, info210, pes210));

    EXPECT_EQ(PES_PACKET_START_CODE_PREFIX, pes210.packet_start_code_prefix);
    EXPECT_EQ(STREAM_ID_pes_audio_stream, pes210.stream_id);
    EXPECT_EQ(2890, pes210.PES_packet_length);
    EXPECT_TRUE(pes210.PES_has_optional_header);
    EXPECT_FALSE(pes210.PES_scrambling_control); // Unscrambled.
    EXPECT_FALSE(pes210.PES_priority);
    EXPECT_TRUE(pes210.data_alignment_indicator);
    EXPECT_TRUE(pes210.copyright);
    EXPECT_TRUE(pes210.original_or_copy);
    EXPECT_EQ(0x02, pes210.PTS_DTS_flags); // only PTS flag (no DTS)
    EXPECT_FALSE(pes210.ESCR_flag);

    EXPECT_FALSE(pes210.ES_rate_flag);
    EXPECT_FALSE(pes210.DSM_trick_mode_flag);
    EXPECT_FALSE(pes210.additional_copy_info_flag);
    EXPECT_FALSE(pes210.PES_CRC_flag);
    EXPECT_FALSE(pes210.PES_extension_flag);
    EXPECT_EQ(7, pes210.PES_header_data_length);
    EXPECT_EQ(1426376378, pes210.pts);
    EXPECT_EQ(-1, pes210.dts); // No DTS...


    EXPECT_EQ(PES_PACKET_START_CODE_PREFIX, pes50.packet_start_code_prefix);
    EXPECT_EQ(STREAM_ID_private_stream_1, pes50.stream_id);
    EXPECT_EQ(1554, pes50.PES_packet_length);
    EXPECT_TRUE(pes50.PES_has_optional_header);
    EXPECT_FALSE(pes50.PES_scrambling_control); // Unscrambled.
    EXPECT_FALSE(pes50.PES_priority);
    EXPECT_TRUE(pes50.data_alignment_indicator);
    EXPECT_FALSE(pes50.copyright);
    EXPECT_FALSE(pes50.original_or_copy);
    EXPECT_EQ(0x02, pes50.PTS_DTS_flags); // only PTS flag (no DTS)
    EXPECT_FALSE(pes50.ESCR_flag);

    EXPECT_FALSE(pes50.ES_rate_flag);
    EXPECT_FALSE(pes50.DSM_trick_mode_flag);
    EXPECT_FALSE(pes50.additional_copy_info_flag);
    EXPECT_FALSE(pes50.PES_CRC_flag);
    EXPECT_FALSE(pes50.PES_extension_flag);
    EXPECT_EQ(15, pes50.PES_header_data_length);
    EXPECT_EQ(161697, pes50.pts);
    EXPECT_EQ(-1, pes50.dts); // No DTS...  
}

/* Parses only PES header from 1 TS-packet */
TEST(TestParsePesPacket, TestParsePesHeader)
{
    TsParser parser;
    PesPacket pes;
    TsPacketInfo info;

    parser.parseTsPacketInfo(pes_packet_1, info);
    EXPECT_EQ(481, info.pid);

    EXPECT_FALSE(parser.collectPes(pes_packet_1, info, pes));
    EXPECT_TRUE(parser.collectPes(pes_packet_1, info, pes));  // Only needed to release PES packet (since unbounded)

    EXPECT_EQ(PES_PACKET_START_CODE_PREFIX, pes.packet_start_code_prefix);
    EXPECT_EQ(STREAM_ID_pes_video_stream, pes.stream_id);
    EXPECT_EQ(0, pes.PES_packet_length);      // Unbounded video packet.
    EXPECT_FALSE(pes.PES_scrambling_control); // Unscrambled.
    EXPECT_FALSE(pes.PES_priority);
    EXPECT_TRUE(pes.data_alignment_indicator);
    EXPECT_FALSE(pes.copyright);
    EXPECT_FALSE(pes.original_or_copy);
    EXPECT_EQ(3, pes.PTS_DTS_flags);
    EXPECT_FALSE(pes.ESCR_flag);

    EXPECT_FALSE(pes.ES_rate_flag);
    EXPECT_FALSE(pes.DSM_trick_mode_flag);
    EXPECT_FALSE(pes.additional_copy_info_flag);
    EXPECT_FALSE(pes.PES_CRC_flag);
    EXPECT_FALSE(pes.PES_extension_flag);
    EXPECT_EQ(10, pes.PES_header_data_length);
    EXPECT_EQ(689094304, pes.pts);
    EXPECT_EQ(689090704, pes.dts);
}

TEST(TsParserTests, TestParsePcr)
{
    TsParser parser;
    TsPacketInfo info;
    parser.parseTsPacketInfo(packet_2, info);
    EXPECT_EQ(599, info.pid);
    EXPECT_EQ(600035807394, info.pcr);
    EXPECT_EQ(-1, info.opcr);

    parser.parseTsPacketInfo(pcr_packet_1, info);
    EXPECT_EQ(0x30, info.pid);
    EXPECT_EQ(31571712, info.pcr);
}

TEST(TsParserTests, parse_descriptor)
{
    try
    {
        TsParser parser;
        TsPacketInfo info;
        int table_id;
        parser.parseTsPacketInfo(pmt_packet_2_1, info);
        EXPECT_EQ(32, info.pid);

        parser.collectTable(pmt_packet_2_1, info, table_id);
        EXPECT_EQ(PSI_TABLE_ID_INCOMPLETE, table_id);

        parser.parseTsPacketInfo(pmt_packet_2_2, info);
        parser.collectTable(pmt_packet_2_2, info, table_id);
        EXPECT_EQ(PSI_TABLE_ID_PMT, table_id);

        auto pmt = parser.parsePmtPacket(info.pid);
        EXPECT_EQ(1, pmt.descriptors.size());
        Descriptor d = pmt.descriptors.back();
        EXPECT_TRUE(d.descriptor_tag == static_cast<uint8_t>(DescriptorTag::metadata_pointer_descriptor));
        EXPECT_EQ(d.descriptor_length, 15); // TODO check this
    }
    catch (std::exception& e)
    {
        FAIL() << "Should not catch exception";
    }
}

TEST(TsParserTests, parse_descriptor_large_pmt)
{
    try
    {
        TsParser parser;
        TsPacketInfo info;
        int table_id;
        parser.parseTsPacketInfo(large_pmt_ts_packet_1, info);
        EXPECT_EQ(50, info.pid);

        parser.collectTable(large_pmt_ts_packet_1, info, table_id);
        EXPECT_EQ(PSI_TABLE_ID_INCOMPLETE, table_id);

        parser.parseTsPacketInfo(large_pmt_ts_packet_2, info);
        parser.collectTable(large_pmt_ts_packet_2, info, table_id);
        EXPECT_EQ(PSI_TABLE_ID_INCOMPLETE, table_id);

        parser.parseTsPacketInfo(large_pmt_ts_packet_2, info);
        parser.collectTable(large_pmt_ts_packet_3, info, table_id);
        EXPECT_EQ(PSI_TABLE_ID_PMT, table_id);

        auto pmt = parser.parsePmtPacket(info.pid);
        EXPECT_EQ(1, pmt.descriptors.size());
    }
    catch (std::exception& e)
    {
        FAIL() << "Should not catch exception";
    }
}
