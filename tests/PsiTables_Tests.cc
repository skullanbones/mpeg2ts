//
// Created by microlab on 6/23/18.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

/// Project files
#include <public/mp2ts.h>

TEST(PsiTablesTests, PsiTableTestComparisonOperator)
{
    PsiTable psi1;
    PsiTable psi2;

    psi1.table_id = 1;
    psi1.section_syntax_indicator = true;
    psi1.section_length = 10;
    psi1.transport_stream_id = 11;
    psi1.version_number = 1;
    psi1.current_next_indicator = false;
    psi1.section_number = 14;
    psi1.last_section_number = 9;
    psi1.CRC_32 = 0;

    // Create identical PsiTable 2
    psi2.table_id = 1;
    psi2.section_syntax_indicator = true;
    psi2.section_length = 10;
    psi2.transport_stream_id = 11;
    psi2.version_number = 1;
    psi2.current_next_indicator = false;
    psi2.section_number = 14;
    psi2.last_section_number = 9;
    psi2.CRC_32 = 0;

    EXPECT_TRUE(psi1 == psi2);

    psi2.table_id = 2;

    EXPECT_FALSE(psi1 == psi2);

    psi2.table_id = 1;
    psi2.CRC_32 = 1;

    EXPECT_FALSE(psi1 == psi2);

    psi2.CRC_32 = 0;
    psi2.section_syntax_indicator = false;

    EXPECT_FALSE(psi1 == psi2);

    psi2.section_syntax_indicator = true;
    psi2.section_length = 9;

    EXPECT_FALSE(psi1 == psi2);

    psi2.section_length = 10;
    psi2.transport_stream_id = 0;

    EXPECT_FALSE(psi1 == psi2);

    psi2.transport_stream_id = 11;
    psi2.version_number = 2;

    EXPECT_FALSE(psi1 == psi2);

    psi2.version_number = 1;
    psi2.current_next_indicator = true;

    EXPECT_FALSE(psi1 == psi2);

    psi2.current_next_indicator = false;
    psi2.section_number = 7;

    EXPECT_FALSE(psi1 == psi2);

    psi2.section_number = 14;
    psi2.last_section_number = 2;
    EXPECT_FALSE(psi1 == psi2);

    psi2.last_section_number = 9;
    EXPECT_TRUE(psi1 == psi2);
}

TEST(PsiTablesTests, PatTableTestComparisonOperatorCRC32)
{
    PatTable pat1;
    PatTable pat2;

    // SPTS
    Program prg;
    prg.program_number = 1;
    prg.program_map_PID = 258;

    pat1.programs.push_back(prg);
    pat2.programs.push_back(prg);

    pat1.CRC_32 = 0;
    pat2.CRC_32 = 0;

    EXPECT_TRUE(pat1 == pat2);

    pat1.CRC_32 = 1;
    pat2.CRC_32 = 0;

    EXPECT_FALSE(pat1 == pat2);
}


TEST(PsiTablesTests, PatTableTestComparisonOperatorNumPrograms)
{
    PatTable pat1;
    PatTable pat2;

    // SPTS
    Program prg;
    prg.program_number = 1;
    prg.program_map_PID = 258;

    Program prg2;
    prg2.program_number = 2;
    prg2.program_map_PID = 4432;

    pat1.programs.push_back(prg);
    pat2.programs.push_back(prg);

    pat1.CRC_32 = 0;
    pat2.CRC_32 = 0;

    EXPECT_TRUE(pat1 == pat2);

    pat2.programs.push_back(prg2);

    EXPECT_FALSE(pat1 == pat2);
}

TEST(PsiTablesTests, PatTableTestComparisonOperatorPrograms)
{
    PatTable pat1;
    PatTable pat2;

    // SPTS
    Program prg;
    prg.program_number = 1;
    prg.program_map_PID = 258;

    Program prg2;
    prg2.program_number = 2;
    prg2.program_map_PID = 4432;

    Program prg3;
    prg3.program_number = 2;
    prg3.program_map_PID = 4433;


    pat1.programs.push_back(prg);
    pat2.programs.push_back(prg);

    pat1.CRC_32 = 0;
    pat2.CRC_32 = 0;

    EXPECT_TRUE(pat1 == pat2);

    pat1.programs.push_back(prg2);
    pat2.programs.push_back(prg3);


    EXPECT_FALSE(pat1 == pat2);
}

TEST(PsiTablesTests, PmtTableTestComparisonOperatorPCR_PID)
{
    PmtTable pmt1;
    PmtTable pmt2;

    StreamTypeHeader stream;
    stream.stream_type = STREAMTYPE_VIDEO_MPEG1;
    stream.elementary_PID = 258;
    stream.ES_info_length = 0;

    pmt1.streams.push_back(stream);
    pmt2.streams.push_back(stream);

    pmt1.PCR_PID = 0;
    pmt1.program_info_length = 0;
    pmt2.PCR_PID = 0;
    pmt2.program_info_length = 0;

    EXPECT_TRUE(pmt1 == pmt2);

    pmt1.PCR_PID = 1;
    pmt2.PCR_PID = 0;

    EXPECT_FALSE(pmt1 == pmt2);

    pmt1.PCR_PID = 0;
    pmt2.program_info_length = 1;
    EXPECT_FALSE(pmt1 == pmt2);
}

TEST(PsiTablesTests, PmtTableTestComparisonOperatorNumStreams)
{
    PmtTable pmt1;
    PmtTable pmt2;

    pmt1.PCR_PID = 0;
    pmt1.program_info_length = 0;
    pmt2.PCR_PID = 0;
    pmt2.program_info_length = 0;

    StreamTypeHeader stream;
    stream.stream_type = STREAMTYPE_VIDEO_MPEG1;
    stream.elementary_PID = 258;
    stream.ES_info_length = 0;

    pmt1.streams.push_back(stream);
    pmt2.streams.push_back(stream);

    EXPECT_TRUE(pmt1 == pmt2);

    StreamTypeHeader stream2;
    stream.stream_type = STREAMTYPE_VIDEO_MPEG2;
    stream.elementary_PID = 32;
    stream.ES_info_length = 0;

    pmt2.streams.push_back(stream2);

    EXPECT_FALSE(pmt1 == pmt2);
}

TEST(PsiTablesTests, StreamTypeHeaderComparisonOperator)
{
    StreamTypeHeader hdr1;
    StreamTypeHeader hdr2;

    hdr1.stream_type = STREAMTYPE_RESERVED;
    hdr1.elementary_PID = 0;
    hdr1.ES_info_length = 0;

    hdr2.stream_type = STREAMTYPE_RESERVED;
    hdr2.elementary_PID = 0;
    hdr2.ES_info_length = 0;

    EXPECT_TRUE(hdr1 == hdr2);

    hdr2.elementary_PID = 1;
    EXPECT_FALSE(hdr1 == hdr2);

    hdr2.elementary_PID = 0;
    hdr2.ES_info_length = 1;
    EXPECT_FALSE(hdr1 == hdr2);

    hdr2.ES_info_length = 0;
    EXPECT_TRUE(hdr1 == hdr2);
}
