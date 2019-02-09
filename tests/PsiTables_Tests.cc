//
// Created by microlab on 6/23/18.
//

#include <iostream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

/// Project files
#include "mpeg2ts.h"

using namespace mpeg2ts;

class PsiTableTest : public ::testing::Test
{
public:
    void SetUp() override
    {
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
    }

    void TearDown() override
    {
    }

    PsiTable psi1;
    PsiTable psi2;
};

class PatTableTest : public ::testing::Test
{
public:
    void SetUp() override
    {
        // SPTS
        Program prg;
        prg.program_number = 1;
        prg.program_map_PID = 258;

        pat1.programs.push_back(prg);
        pat2.programs.push_back(prg);

        pat1.CRC_32 = 0;
        pat2.CRC_32 = 0;
    }

    void TearDown() override
    {
    }

    PatTable pat1;
    PatTable pat2;
};


class PmtTableTest : public ::testing::Test
{
public:
    void SetUp() override
    {
        stream.stream_type = STREAMTYPE_VIDEO_MPEG1;
        stream.elementary_PID = 258;
        stream.ES_info_length = 0;

        pmt1.streams.push_back(stream);
        pmt2.streams.push_back(stream);

        pmt1.PCR_PID = 0;
        pmt2.PCR_PID = 0;
        pmt1.program_info_length = 0;
        pmt2.program_info_length = 0;

        ///
        pmt3.streams.push_back(stream);
        pmt4.streams.push_back(stream);

        pmt3.PCR_PID = 0;
        pmt4.PCR_PID = 0;
        pmt3.program_info_length = 0;
        pmt4.program_info_length = 0;
    }

    void TearDown() override
    {
    }

    PmtTable pmt1;
    PmtTable pmt2;
    PmtTable pmt3;
    PmtTable pmt4;
    StreamTypeHeader stream;
};

TEST_F(PsiTableTest, PsiTableTestComparisonOperator_tableid)
{
    EXPECT_TRUE(psi1 == psi2) << "2 identical PSI tables should be equal.";
    EXPECT_FALSE(psi1 != psi2);

    psi2.table_id = 2;

    EXPECT_FALSE(psi1 == psi2) << "table_id not equal hence should not be equal.";
    EXPECT_TRUE(psi1 != psi2);
}

TEST_F(PsiTableTest, PsiTableTestComparisonOperator_crc32)
{
    EXPECT_TRUE(psi1 == psi2) << "2 identical PSI tables should be equal.";
    EXPECT_FALSE(psi1 != psi2);

    psi2.CRC_32 = 1;

    EXPECT_FALSE(psi1 == psi2) << "table_id and CRC_32 not equal hence should not be equal.";
    EXPECT_TRUE(psi1 != psi2);
}

TEST_F(PsiTableTest, PsiTableTestComparisonOperator_section_syntax_indicator)
{
    EXPECT_TRUE(psi1 == psi2) << "2 identical PSI tables should be equal.";
    EXPECT_FALSE(psi1 != psi2);

    psi2.section_syntax_indicator = false;

    EXPECT_FALSE(psi1 == psi2);
    EXPECT_TRUE(psi1 != psi2);
}

TEST_F(PsiTableTest, PsiTableTestComparisonOperator_section_length)
{
    EXPECT_TRUE(psi1 == psi2) << "2 identical PSI tables should be equal.";
    EXPECT_FALSE(psi1 != psi2);

    psi2.section_length = 9;

    EXPECT_FALSE(psi1 == psi2);
    EXPECT_TRUE(psi1 != psi2);
}

TEST_F(PsiTableTest, PsiTableTestComparisonOperator_transport_stream_id)
{
    EXPECT_TRUE(psi1 == psi2) << "2 identical PSI tables should be equal.";
    EXPECT_FALSE(psi1 != psi2);

    psi2.transport_stream_id = 0;

    EXPECT_FALSE(psi1 == psi2);
    EXPECT_TRUE(psi1 != psi2);
}

TEST_F(PsiTableTest, PsiTableTestComparisonOperator_version_number)
{
    EXPECT_TRUE(psi1 == psi2) << "2 identical PSI tables should be equal.";
    EXPECT_FALSE(psi1 != psi2);

    psi2.version_number = 2;

    EXPECT_FALSE(psi1 == psi2);
    EXPECT_TRUE(psi1 != psi2);
}

TEST_F(PsiTableTest, PsiTableTestComparisonOperator_current_next_indicator)
{
    EXPECT_TRUE(psi1 == psi2) << "2 identical PSI tables should be equal.";
    EXPECT_FALSE(psi1 != psi2);

    psi2.current_next_indicator = true;

    EXPECT_FALSE(psi1 == psi2);
    EXPECT_TRUE(psi1 != psi2);

    psi2.current_next_indicator = false;
    psi2.section_number = 7;

    EXPECT_FALSE(psi1 == psi2);
    EXPECT_TRUE(psi1 != psi2);

    psi2.section_number = 14;
    psi2.last_section_number = 2;
    EXPECT_FALSE(psi1 == psi2);
    EXPECT_TRUE(psi1 != psi2);

    psi2.last_section_number = 9;
    EXPECT_TRUE(psi1 == psi2);
    EXPECT_FALSE(psi1 != psi2);
}

TEST_F(PatTableTest, test_comparison_operator_CRC32)
{
    EXPECT_TRUE(pat1 == pat2);
    EXPECT_FALSE(pat1 != pat2);

    pat1.CRC_32 = 1;
    pat2.CRC_32 = 0;

    EXPECT_FALSE(pat1 == pat2);
    EXPECT_TRUE(pat1 != pat2);
}

TEST_F(PatTableTest, test_comparison_operator_num_programs)
{
    EXPECT_TRUE(pat1 == pat2);
    EXPECT_FALSE(pat1 != pat2);

    Program prg2;
    prg2.program_number = 2;
    prg2.program_map_PID = 4432;

    pat2.programs.push_back(prg2);

    EXPECT_FALSE(pat1 == pat2);
    EXPECT_TRUE(pat1 != pat2);
}

TEST(PsiTablesTests, test_comparison_operator_programs)
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
    EXPECT_FALSE(pat1 != pat2);

    pat1.programs.push_back(prg2);
    pat2.programs.push_back(prg3);

    EXPECT_FALSE(pat1 == pat2);
    EXPECT_TRUE(pat1 != pat2);
}

TEST_F(PmtTableTest, PmtTableTestComparisonOperatorPCR_PID)
{
    EXPECT_TRUE(pmt1 == pmt2);
    EXPECT_FALSE(pmt1 != pmt2);

    pmt1.PCR_PID = 1;
    pmt2.PCR_PID = 0;

    EXPECT_FALSE(pmt1 == pmt2);
    EXPECT_TRUE(pmt1 != pmt2);

    pmt1.CRC_32 = 3;
    pmt2.CRC_32 = 1;

    EXPECT_FALSE(pmt1 == pmt2);
    EXPECT_TRUE(pmt1 != pmt2);

    pmt1.PCR_PID = 0;
    pmt2.program_info_length = 1;
    EXPECT_FALSE(pmt1 == pmt2);
    EXPECT_TRUE(pmt1 != pmt2);
}

TEST_F(PmtTableTest, PmtTableTestComparisonOperatorNumStreams)
{
    EXPECT_TRUE(pmt1 == pmt2);
    EXPECT_FALSE(pmt1 != pmt2);

    StreamTypeHeader stream2;
    stream.stream_type = STREAMTYPE_VIDEO_MPEG2;
    stream.elementary_PID = 32;
    stream.ES_info_length = 0;

    pmt2.streams.push_back(stream2);

    EXPECT_FALSE(pmt1 == pmt2);
    EXPECT_TRUE(pmt1 != pmt2);
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

TEST_F(PsiTableTest, test_psitable_stream_operator)
{
    PsiTable d;
    std::stringstream out;
    out << d;

    std::string correct_str = "\n-------------PsiTable-------------\n"
                              "table_id:0\n"
                              "section_syntax_indicator: 0\n"
                              "section_length: 0\n"
                              "transport_stream_id: 0\n"
                              "version_number: 0\n"
                              "current_next_indicator: 0\n"
                              "section_number: 0\n"
                              "last_section_number: 0\n";


    EXPECT_EQ(out.str(), correct_str) << "operator<< does not give correct output";
}

TEST_F(PsiTableTest, test_pattable_stream_operator)
{
    PatTable d;
    Program p = { 1, 10, ProgramType::PMT };
    d.programs.push_back(p);
    std::stringstream out;
    out << d;

    std::string correct_str = "\n-------------PatTable-------------\n"
                              "programs.size(): 1\n"
                              "-------------program 0--------------\n"
                              "program_number: 1\n"
                              "program_map_PID: 10\n";

    EXPECT_EQ(out.str(), correct_str) << "operator<< does not give correct output";
}

TEST_F(PsiTableTest, test_pattable_stream_operator_nit)
{
    PatTable d;
    Program p = { 1, 10, ProgramType::NIT };
    d.programs.push_back(p);
    std::stringstream out;
    out << d;

    std::string correct_str = "\n-------------PatTable-------------\n"
                              "programs.size(): 1\n"
                              "-------------program 0--------------\n"
                              "program_number: 1\n"
                              "network_PID: 10\n";

    EXPECT_EQ(out.str(), correct_str) << "operator<< does not give correct output";
}

TEST_F(PmtTableTest, test_pmttable_stream_operator)
{
    PmtTable d;
    StreamTypeHeader hdr = { STREAMTYPE_RESERVED, 10, 1 };
    d.streams.push_back(hdr);
    std::stringstream out;
    out << d;

    std::string correct_str = "\n-------------PmtTable-------------\n"
                              "PCR_PID: 0\n"
                              "program_info_length: 0\n"
                              "streams.size(): 1\n"
                              "-------------stream 0--------------\n"
                              "stream_type: STREAMTYPE_RESERVED,  (0)\n"
                              "elementary_PID: 10\n"
                              "ES_info_length: 1\n";

    EXPECT_EQ(out.str(), correct_str) << "operator<< does not give correct output";
}

TEST(StreamTypeHeader, test_stream_type_hdr)
{
    StreamTypeHeader hdr = { STREAMTYPE_RESERVED, 10, 1 };
    std::stringstream out;
    out << hdr;

    std::string correct_str = "\n-------------StreamTypeHeader-------------\n"
                              "stream_type: STREAMTYPE_RESERVED,  (0)\n"
                              "elementary_PID: 10\n"
                              "ES_info_length: 1\n";

    EXPECT_EQ(out.str(), correct_str) << "operator<< does not give correct output";
}
