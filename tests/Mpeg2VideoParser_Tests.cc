
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


TEST_F(Mpeg2VideoTest, test_bare_findStartCodes)
{
    std::vector<uint8_t> testVec = { 0, 0, 1 };
    std::vector<uint8_t> testVec2 = { 0, 0, 0, 1 };

    std::vector<std::size_t> ret = parser.findStartCodes(testVec);
    std::vector<std::size_t> ret2 = parser.findStartCodes(testVec2);

    EXPECT_EQ(1, ret.size());
    EXPECT_EQ(0, ret[0]);
    EXPECT_EQ(1, ret2.size());
    EXPECT_EQ(1, ret2[0]);
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


TEST_F(Mpeg2VideoTest, test_findStartCodes_sequence_header)
{
    std::vector<std::size_t> ret = parser.findStartCodes(h262_with_sequence_header_code);

    EXPECT_EQ(6, ret.size());
    EXPECT_EQ(202, ret[0]);
    EXPECT_EQ(278, ret[1]);
    EXPECT_EQ(288, ret[2]);
    EXPECT_EQ(296, ret[3]);
    EXPECT_EQ(304, ret[4]);
    EXPECT_EQ(313, ret[5]);
}


TEST_F(Mpeg2VideoTest, test_parse_sequence_header)
{
    std::vector<EsInfoMpeg2> ret = parser.parse(h262_with_sequence_header_code);

    for (const EsInfoMpeg2& esinfo : ret)
    {
        if (esinfo.type == mpeg2::Mpeg2Type::SliceCode)
        {
            EXPECT_EQ(esinfo.slice.picType, 1);
            EXPECT_EQ(esinfo.msg, "I");
        }
        else if (esinfo.type == mpeg2::Mpeg2Type::SequenceHeader)
        {
            EXPECT_EQ(esinfo.sequence.width, 704);
            EXPECT_EQ(esinfo.sequence.height, 576);
            EXPECT_EQ(esinfo.sequence.framerate, "25");
            EXPECT_EQ(esinfo.sequence.aspect, "3x4");
        }
    }
}


TEST_F(Mpeg2VideoTest, test_findStartCodes_sequence_header_2)
{
    std::vector<std::size_t> ret = parser.findStartCodes(h262_with_sequence_header_code_2);

    EXPECT_EQ(8, ret.size());
    EXPECT_EQ(46, ret[0]);
    EXPECT_EQ(354, ret[1]);
    EXPECT_EQ(722, ret[2]);
    EXPECT_EQ(798, ret[3]);
    EXPECT_EQ(808, ret[4]);
    EXPECT_EQ(816, ret[5]);
    EXPECT_EQ(824, ret[6]);
    EXPECT_EQ(833, ret[7]);
}

TEST_F(Mpeg2VideoTest, test_parse_sequence_header_2)
{
    std::vector<EsInfoMpeg2> ret = parser.parse(h262_with_sequence_header_code_2);

    for (const EsInfoMpeg2& esinfo : ret)
    {
        // std::cout << "mpeg2 picture: " << i->picture << " " << i->msg;
        if (esinfo.type == mpeg2::Mpeg2Type::SliceCode)
        {
            EXPECT_EQ(esinfo.slice.picType, 1);
            EXPECT_EQ(esinfo.msg, "I");
            std::cout << "mpeg2 picture type: " << esinfo.slice.picType << " " << esinfo.msg;
        }
        else if (esinfo.type == mpeg2::Mpeg2Type::SequenceHeader)
        {
            EXPECT_EQ(esinfo.sequence.width, 704);
            EXPECT_EQ(esinfo.sequence.height, 576);
            EXPECT_EQ(esinfo.sequence.framerate, "25");
            EXPECT_EQ(esinfo.sequence.aspect, "3x4");
            std::cout << esinfo.sequence.width << " x " << esinfo.sequence.height
                      << ", aspect: " << esinfo.sequence.aspect
                      << ", frame rate: " << esinfo.sequence.framerate;
        }
    }
}
