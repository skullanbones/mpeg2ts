
#include <gmock/gmock.h>
#include <gtest/gtest.h>

/// Project files CUT (Class Under Test)
#include <mpeg2vid/Mpeg2VideoParser.h>

using namespace mpeg2;

// Not sure this should be supposed to be correct
// NAL unit start codes is 4 bytes not 3...
TEST(Mpeg2VideoTests, Parse0)
{
    std::vector<uint8_t> testVec = { 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec.data(), testVec.size());

    EXPECT_EQ(1, parser.m_foundStartCodes);
    EXPECT_EQ(1, parser.m_indexes.size());
    EXPECT_EQ(2, parser.m_indexes[0]);
}

TEST(Mpeg2VideoTests, test_getFirstOne)
{
    std::vector<uint8_t> testVec = { 0, 0, 1 };
    std::vector<uint8_t> testVec2 = { 0, 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    const uint8_t* ret = parser.getFirstOne(testVec.data(), testVec.size());
    const uint8_t* ret2 = parser.getFirstOne(testVec2.data(), testVec2.size());

    EXPECT_EQ(1, *ret);
    EXPECT_EQ(ret - testVec.data(), 2);
    EXPECT_EQ(0, parser.m_indexes.size()); // we didn't parse

    EXPECT_EQ(1, *ret2);
    EXPECT_EQ(ret2 - testVec2.data(), 3);
}

TEST(Mpeg2VideoTests, Parse1)
{
    std::vector<uint8_t> testVec = { 0, 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec.data(), testVec.size());

    EXPECT_EQ(1, parser.m_foundStartCodes);
    EXPECT_EQ(1, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, Parse2)
{
    std::vector<uint8_t> testVec = { 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec.data(), testVec.size());

    EXPECT_EQ(0, parser.m_foundStartCodes);
    EXPECT_EQ(0, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, Parse3)
{
    std::vector<uint8_t> testVec = { 0, 0, 0 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec.data(), testVec.size());

    EXPECT_EQ(0, parser.m_foundStartCodes);
    EXPECT_EQ(0, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, Parse4)
{
    std::vector<uint8_t> testVec1 = { 0, 0, 0 };
    std::vector<uint8_t> testVec2 = { 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec1.data(), testVec1.size());
    parser.parse(testVec2.data(), testVec2.size());

    EXPECT_EQ(1, parser.m_foundStartCodes);
    EXPECT_EQ(1, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, Parse5)
{
    std::vector<uint8_t> testVec1 = { 0, 0 };
    std::vector<uint8_t> testVec2 = { 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec1.data(), testVec1.size());
    parser.parse(testVec2.data(), testVec2.size());

    EXPECT_EQ(1, parser.m_foundStartCodes); // TODO seem its flaky...
    EXPECT_EQ(1, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, Parse6)
{
    std::vector<uint8_t> testVec1 = { 0 };
    std::vector<uint8_t> testVec2 = { 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec1.data(), testVec1.size());
    parser.parse(testVec2.data(), testVec2.size());

    EXPECT_EQ(1, parser.m_foundStartCodes);
    EXPECT_EQ(1, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, Parse7)
{
    std::vector<uint8_t> testVec = { 1, 2, 3, 0, 0, 0, 1, 4, 5, 6, 7, 0, 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec.data(), testVec.size());

    EXPECT_EQ(2, parser.m_foundStartCodes);
    EXPECT_EQ(2, parser.m_indexes.size());
}

TEST(Mpeg2VideoTests, Parse8)
{
    std::vector<uint8_t> testVec = { 1, 2, 3, 0, 1, 0, 0, 1 };
    Mpeg2VideoEsParser parser;

    parser.parse(testVec.data(), testVec.size());

    EXPECT_EQ(1, parser.m_foundStartCodes);
    EXPECT_EQ(1, parser.m_indexes.size());
}
