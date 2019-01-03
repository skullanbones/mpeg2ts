
#include <gmock/gmock.h>
#include <gtest/gtest.h>

/// Project files CUT (Class Under Test)
#include "mpeg2vid/Mpeg2VideoParser.h"

#include "CodecTestData.h"

using namespace mpeg2;


class Mpeg2VideoTest : public ::testing::Test
{
public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
    Mpeg2VideoEsParser parser;
};

// Not sure this should be supposed to be correct
// NAL unit start codes is 4 bytes not 3...??
TEST_F(Mpeg2VideoTest, Parse0)
{
    std::vector<uint8_t> testVec = { 0, 0, 1 };

    parser.parse(testVec);

    EXPECT_EQ(1, parser.m_indexes.size());
}


TEST_F(Mpeg2VideoTest, test_bare_findStartCodes)
{
    std::vector<uint8_t> testVec = { 0, 0, 1 };
    std::vector<uint8_t> testVec2 = { 0, 0, 0, 1 };

    std::vector<std::size_t> ret = parser.findStartCodes(testVec);

    EXPECT_EQ(1, ret.size());
    EXPECT_EQ(0, ret[0]);
}


TEST_F(Mpeg2VideoTest, test_findStartCodes_1)
{
    std::vector<uint8_t> testVec = { 0, 0, 0, 1 };

    std::vector<std::size_t> ret = parser.findStartCodes(testVec);

    EXPECT_EQ(1, ret.size());
    EXPECT_EQ(1, ret[0]);
}


TEST_F(Mpeg2VideoTest, test_findStartCodes_2)
{
    std::vector<uint8_t> testVec = { 0, 1 };

    std::vector<std::size_t> ret = parser.findStartCodes(testVec);

    EXPECT_EQ(0, ret.size());
}


TEST_F(Mpeg2VideoTest, test_findStartCodes_3)
{
    std::vector<uint8_t> testVec = { 0, 0, 0 };

    std::vector<std::size_t> ret = parser.findStartCodes(testVec);

    EXPECT_EQ(0, ret.size());
}


TEST_F(Mpeg2VideoTest, test_findStartCodes_4)
{
    std::vector<uint8_t> testVec1 = { 0, 0, 0 };
    std::vector<uint8_t> testVec2 = { 1 };

    std::vector<std::size_t> ret = parser.findStartCodes(testVec1);
    EXPECT_EQ(0, ret.size());
}


TEST_F(Mpeg2VideoTest, test_findStartCodes_5)
{
    std::vector<uint8_t> testVec1 = { 0, 0 };
    std::vector<uint8_t> testVec2 = { 0, 1 };

    std::vector<std::size_t> ret = parser.findStartCodes(testVec1);

    EXPECT_EQ(0, ret.size());
}


TEST_F(Mpeg2VideoTest, test_findStartCodes_6)
{
    std::vector<uint8_t> testVec1 = { 0 };
    std::vector<uint8_t> testVec2 = { 0, 0, 1 };

    std::vector<std::size_t> ret = parser.findStartCodes(testVec1);

    EXPECT_EQ(0, ret.size());
}


TEST_F(Mpeg2VideoTest, test_findStartCodes_7)
{
    std::vector<uint8_t> testVec = { 1, 2, 3, 0, 0, 0, 1, 4, 5, 6, 7, 0, 0, 0, 1 };

    std::vector<std::size_t> ret = parser.findStartCodes(testVec);

    EXPECT_EQ(2, ret.size());
    EXPECT_EQ(4, ret[0]);
    EXPECT_EQ(12, ret[1]);
}


TEST_F(Mpeg2VideoTest, test_findStartCodes_8)
{
    std::vector<uint8_t> testVec = { 1, 2, 3, 0, 1, 0, 0, 1 };

    std::vector<std::size_t> ret = parser.findStartCodes(testVec);

    EXPECT_EQ(1, ret.size());
    EXPECT_EQ(5, ret[0]);
}
