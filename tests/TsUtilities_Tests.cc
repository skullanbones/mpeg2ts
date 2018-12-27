
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

// Project files
#include "public/mpeg2ts.h"
#include "TsPacketTestData.h"

// Class Under Test (CUT)
#include "public/TsUtilities.h"

using namespace mpeg2ts;
using namespace tsutil;

class TsUtilitiesTest : public ::testing::Test
{
public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    TsUtilities m_tsUtil;
};

TEST_F(TsUtilitiesTest, test_parseTransportStreamData_1)
{
    m_tsUtil.parseTransportStreamData(pat_packet_1, sizeof(pat_packet_1));
    mpeg2ts::PatTable pat;
    pat =  m_tsUtil.getPatTable();
    std::vector<uint16_t> pmtPids;
    pmtPids = m_tsUtil.getPmtPids();
    EXPECT_EQ(pmtPids.size(), 1);
    EXPECT_EQ(pmtPids.at(0), 598);
}

TEST_F(TsUtilitiesTest, test_parseTransportStreamData_2)
{
    m_tsUtil.parseTransportStreamData(pat_packet_2, sizeof(pat_packet_2));
    mpeg2ts::PatTable pat;
    pat =  m_tsUtil.getPatTable();
    const int kNumPmts = 17;
    EXPECT_EQ(pat.programs.size(), kNumPmts); // FOUND bug!
    std::vector<uint16_t> pmtPids;
    pmtPids = m_tsUtil.getPmtPids();
    EXPECT_EQ(pmtPids.size(), kNumPmts - 1); // BUG!!! ????
    EXPECT_EQ(pmtPids.at(0), 5180); // Check description in TsPacketTestData.h
    EXPECT_EQ(pmtPids.at(1), 5050);
    EXPECT_EQ(pmtPids.at(2), 5070);
    EXPECT_EQ(pmtPids.at(3), 5540);
    EXPECT_EQ(pmtPids.at(4), 1240);
    EXPECT_EQ(pmtPids.at(5), 5640);
    EXPECT_EQ(pmtPids.at(6), 1010);
    EXPECT_EQ(pmtPids.at(7), 5840);
    EXPECT_EQ(pmtPids.at(8), 5800);
    EXPECT_EQ(pmtPids.at(9), 1290);
    EXPECT_EQ(pmtPids.at(10), 1280);
    EXPECT_EQ(pmtPids.at(11), 870);
    EXPECT_EQ(pmtPids.at(12), 1810);
    EXPECT_EQ(pmtPids.at(13), 3010);
    EXPECT_EQ(pmtPids.at(14), 3020);
    EXPECT_EQ(pmtPids.at(15), 3030);
}

