
#include <gmock/gmock.h>
#include <gtest/gtest.h>

/// Project files CUT (Class Under Test)
#include <mpeg2vid/Mpeg2VideoParser.h>


TEST(Mpeg2VideoTests, Parse1)
{
    std::vector<uint8_t> testVec = {0,0,0,1};
    Mpeg2VideoEsParser parser;

    parser(testVec.data(), testVec.size());
    
    EXPECT_EQ(1, parser.foundStartCodes);
}

TEST(Mpeg2VideoTests, Parse2)
{
    std::vector<uint8_t> testVec = {0,0,1};
    Mpeg2VideoEsParser parser;

    parser(testVec.data(), testVec.size());

    EXPECT_EQ(0, parser.foundStartCodes);
}

TEST(Mpeg2VideoTests, Parse3)
{
    std::vector<uint8_t> testVec = {0,0,0};
    Mpeg2VideoEsParser parser;

    parser(testVec.data(), testVec.size());

    EXPECT_EQ(0, parser.foundStartCodes);
}

TEST(Mpeg2VideoTests, Parse4)
{
    std::vector<uint8_t> testVec1 = {0,0,0};
    std::vector<uint8_t> testVec2 = {1};
    Mpeg2VideoEsParser parser;

    parser(testVec1.data(), testVec1.size());
    parser(testVec2.data(), testVec2.size());

    EXPECT_EQ(1, parser.foundStartCodes);
}

TEST(Mpeg2VideoTests, Parse5)
{
    std::vector<uint8_t> testVec1 = {0,0};
    std::vector<uint8_t> testVec2 = {0,1};
    Mpeg2VideoEsParser parser;

    parser(testVec1.data(), testVec1.size());
    parser(testVec2.data(), testVec2.size());
    
    EXPECT_EQ(1, parser.foundStartCodes);
}

TEST(Mpeg2VideoTests, Parse6)
{
    std::vector<uint8_t> testVec1 = {0};
    std::vector<uint8_t> testVec2 = {0,0,1};
    Mpeg2VideoEsParser parser;

    parser(testVec1.data(), testVec1.size());
    parser(testVec2.data(), testVec2.size());
    
    EXPECT_EQ(1, parser.foundStartCodes);
}

TEST(Mpeg2VideoTests, Parse7)
{
    std::vector<uint8_t> testVec = {1,2,3,0,0,0,1,4,5,6,7,0,0,0,1};
    Mpeg2VideoEsParser parser;

    parser(testVec.data(), testVec.size());

    EXPECT_EQ(2, parser.foundStartCodes);
}

TEST(Mpeg2VideoTests, Parse8)
{
    std::vector<uint8_t> testVec = {1,2,3,0,0,1,0,0,0,1};
    Mpeg2VideoEsParser parser;

    parser(testVec.data(), testVec.size());

    EXPECT_EQ(1, parser.foundStartCodes);
}
