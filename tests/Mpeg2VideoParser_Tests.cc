
#include <gmock/gmock.h>
#include <gtest/gtest.h>

/// Project files CUT (Class Under Test)
#include <mpeg2vid/Mpeg2VideoParser.h>

using namespace mpeg2;

// Not sure this should be supposed to be correct
// NAL unit start codes is 4 bytes not 3...??
TEST(Mpeg2VideoTests, Parse0)
{
    std::vector<uint8_t> testVec = { 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec);

    EXPECT_EQ(0, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, test_findStartCodes)
{
    std::vector<uint8_t> testVec = { 0, 0, 1 };
    std::vector<uint8_t> testVec2 = { 0, 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    std::vector<std::size_t> ret = parser.findStartCodes(testVec);
    std::vector<std::size_t> ret2 = parser.findStartCodes(testVec2);

    EXPECT_EQ(0, ret.size());
    // TODO check positions
    EXPECT_EQ(1, ret2.size());
}

TEST(Mpeg2VideoTests, test_findStartCodes_1)
{
    std::vector<uint8_t> testVec = { 0, 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.findStartCodes(testVec);

    EXPECT_EQ(1, parser.m_indexes.size());
    EXPECT_EQ(0, parser.m_indexes[0]);
}

TEST(Mpeg2VideoTests, Parse1)
{
    std::vector<uint8_t> testVec = { 0, 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec);

    EXPECT_EQ(1, parser.m_indexes.size());
    EXPECT_EQ(0, parser.m_indexes[0]);
}

TEST(Mpeg2VideoTests, test_findStartCodes_2)
{
    std::vector<uint8_t> testVec = { 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.findStartCodes(testVec);

    EXPECT_EQ(0, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, Parse2)
{
    std::vector<uint8_t> testVec = { 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec);

    EXPECT_EQ(0, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, test_findStartCodes_3)
{
    std::vector<uint8_t> testVec = { 0, 0, 0 };
    Mpeg2VideoEsParser parser;

    parser.findStartCodes(testVec);

    EXPECT_EQ(0, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, Parse3)
{
    std::vector<uint8_t> testVec = { 0, 0, 0 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec);

    EXPECT_EQ(0, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, test_findStartCodes_4)
{
    std::vector<uint8_t> testVec1 = { 0, 0, 0 };
    std::vector<uint8_t> testVec2 = { 1 };
    Mpeg2VideoEsParser parser;

    parser.findStartCodes(testVec1);
    EXPECT_EQ(0, parser.m_indexes.size());
    parser.findStartCodes(testVec2);
    EXPECT_EQ(0, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, Parse4)
{
    std::vector<uint8_t> testVec1 = { 0, 0, 0 };
    std::vector<uint8_t> testVec2 = { 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec1);
    EXPECT_EQ(0, parser.m_indexes.size());
    parser.parse(testVec2);
    EXPECT_EQ(0, parser.m_indexes.size());
}


TEST(Mpeg2VideoTests, test_findStartCodes_5)
{
    std::vector<uint8_t> testVec1 = { 0, 0 };
    std::vector<uint8_t> testVec2 = { 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.findStartCodes(testVec1);
    parser.findStartCodes(testVec2);

    EXPECT_EQ(0, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, Parse5)
{
    std::vector<uint8_t> testVec1 = { 0, 0 };
    std::vector<uint8_t> testVec2 = { 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec1);
    parser.parse(testVec2);

    EXPECT_EQ(0, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, test_findStartCodes_6)
{
    std::vector<uint8_t> testVec1 = { 0 };
    std::vector<uint8_t> testVec2 = { 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.findStartCodes(testVec1);
    parser.findStartCodes(testVec2);

    EXPECT_EQ(0, parser.m_indexes.size());
}


TEST(Mpeg2VideoTests, Parse6)
{
    std::vector<uint8_t> testVec1 = { 0 };
    std::vector<uint8_t> testVec2 = { 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec1);
    parser.parse(testVec2);

    EXPECT_EQ(0, parser.m_indexes.size());
}


TEST(Mpeg2VideoTests, test_findStartCodes_7)
{
    std::vector<uint8_t> testVec = { 1, 2, 3, 0, 0, 0, 1, 4, 5, 6, 7, 0, 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.findStartCodes(testVec);

    EXPECT_EQ(2, parser.m_indexes.size());
    EXPECT_EQ(3, parser.m_indexes[0]);
    EXPECT_EQ(11, parser.m_indexes[1]);
}

TEST(Mpeg2VideoTests, Parse7)
{
    std::vector<uint8_t> testVec = { 1, 2, 3, 0, 0, 0, 1, 4, 5, 6, 7, 0, 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec);

    EXPECT_EQ(2, parser.m_indexes.size());
    EXPECT_EQ(3, parser.m_indexes[0]);
    EXPECT_EQ(11, parser.m_indexes[1]);
}

TEST(Mpeg2VideoTests, test_findStartCodes_8)
{
    std::vector<uint8_t> testVec = { 1, 2, 3, 0, 1, 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.findStartCodes(testVec);

    EXPECT_EQ(0, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, Parse8)
{
    std::vector<uint8_t> testVec = { 1, 2, 3, 0, 1, 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec);

    EXPECT_EQ(0, parser.m_indexes.size());
}
