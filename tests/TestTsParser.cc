//
// Created by microlab on 2/24/18.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

/// Project files
#include "TsPacketInfo.h"
#include "TsPacketTestData.h"
#include "TsParser.h"

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
    // std::cout << info.toString();

    // std::cout << hdr.toString();
    EXPECT_EQ(0, info.pcr);
    EXPECT_EQ(0, info.opcr);
    EXPECT_EQ(47, info.payloadSize);
    EXPECT_EQ(141, info.payloadStartOffset);
    EXPECT_TRUE(info.hasAdaptationField);

    // TODO add more tests
}

TEST(TsParserTests, CheckPid)
{
    TsParser parser;
    // TsHeader hdr = parser.parseTsHeader(packet_1);

    TsPacketInfo info;
    parser.parseTsPacketInfo(packet_3, info);
    EXPECT_EQ(289, info.pid);
    parser.parseTsPacketInfo(packet_4, info);
    EXPECT_EQ(481, info.pid);
    // TODO add more tests...
}

TEST(TsParserTests, CheckParsePatPacket)
{
    TsParser parser;
    // TsHeader hdr = parser.parseTsHeader(packet_1);

    PsiTable pat;
    TsPacketInfo info;
    parser.parseTsPacketInfo(pat_packet_1, info);
    pat = parser.parsePatPacket(pat_packet_1, info);
    EXPECT_EQ(TS_PACKET_PID_PAT, info.pid);
    EXPECT_EQ(PSI_TABLE_ID_PAT, pat.table_id);
    //    EXPECT_EQ(598, pat.network_PID);

    parser.parseTsPacketInfo(pat_packet_2, info);
    pat = parser.parsePatPacket(pat_packet_2, info);
    EXPECT_EQ(TS_PACKET_PID_PAT, info.pid);
    EXPECT_EQ(PSI_TABLE_ID_PAT, pat.table_id);

    EXPECT_EQ(0x03fd, pat.transport_stream_id);
    EXPECT_EQ(0, pat.section_number);
    EXPECT_EQ(0, pat.last_section_number);
}

TEST(TsParserTests, CheckParseTsHeader)
{
    TsParser parser;
    TsHeader hdr = parser.parseTsHeader(packet_3);

    EXPECT_EQ(289, hdr.PID);
    //parser.parseTsPacketInfo(packet_4, info);
    //EXPECT_EQ(481, info.pid);
    // TODO add more tests...
}

TEST(MathTest, TwoPlusTwoEqualsFour)
{
    EXPECT_EQ(2 + 2, 4);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
