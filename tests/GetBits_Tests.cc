#include <gmock/gmock.h>
#include <gtest/gtest.h>

/// Project files CUT (Code Under Test)
#include "GetBits.h"

/*!
 * Dummy data for testing bits parser
 */
uint8_t testData[] = { 0x47, 0x40, 0x63, 0x15, 0x00, 0x02, 0xb0, 0x5b, 0x00, 0x01, 0xd1, 0x00, 0x00,
                       0xe0, 0x65, 0xf0, 0x00, 0x02, 0xe0, 0x65, 0xf0, 0x03, 0x52, 0x01, 0x01, 0x03,
                       0xe0, 0x69, 0xf0, 0x09, 0x0a, 0x04, 0x63, 0x7a, 0x65, 0x00, 0x52, 0x01, 0x0b,
                       0x03, 0xe0, 0x73, 0xf1, 0x09, 0x0a, 0x04, 0x65, 0x6e, 0x67, 0x00, 0x52, 0x01,
                       0x0c, 0x06, 0xe1, 0x2e, 0xf0, 0x25, 0x45, 0x1c, 0x01, 0x1a, 0xe7, 0xe8, 0xe9,
                       0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf4, 0xf5, 0xf6, 0xc7, 0xc8, 0xc9, 0xca,
                       0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd1, 0xd3, 0xd4, 0xd5, 0xd6, 0x56, 0x05, 0x65,
                       0x6e, 0x67, 0x09, 0x00, 0xad, 0x95, 0x17, 0xf2, 0xff, 0xff, 0xff, 0xff, 0xff,
                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff };


/// size 10
uint8_t limitedData[] = { 0x47, 0x40, 0x63, 0x15, 0x00, 0x02, 0xb0, 0x5b, 0x00, 0x01 };

/// Test fixture
struct GetBitsTest : public ::testing::Test
{
    void SetUp() override
    {
        parser.resetBits(testData, sizeof(testData));
        empty_parser.resetBits({ 0 }, 0);
    }

    template <typename Callable>
    void ExpectCorrectException(const Callable& f, const GetBitsException& expected)
    {
        try
        {
            f();
            FAIL() << "Expected GetBitsException";
        }
        catch (GetBitsException const& e)
        {
            ASSERT_STREQ(e.what(), expected.what())
            << "Wrong exception message, expected: " << expected.what();
        }
        catch (...)
        {
            FAIL() << "Expected GetBitsException";
        }
    }

    GetBits parser;
    GetBits empty_parser;
};


/*!
 * Test basic bit parser functionality
 */
TEST_F(GetBitsTest, TestGetBits)
{
    EXPECT_EQ(sizeof(testData), 188);

    EXPECT_EQ(0x47, parser.getBits(8));
    EXPECT_EQ(0x40, parser.getBits(8));
    EXPECT_EQ(0x63, parser.getBits(8));
    EXPECT_EQ(0x15, parser.getBits(8));
    EXPECT_EQ(0x00, parser.getBits(8));
    EXPECT_EQ(0x02, parser.getBits(8));
    EXPECT_EQ(0xb0, parser.getBits(8));
    EXPECT_EQ(0x5b, parser.getBits(8));
}

/*!
 * Tests that we can alternate between reading and skipping bits
 */
TEST_F(GetBitsTest, TestSkipBytesAlternating)
{
    EXPECT_EQ(sizeof(testData), 188);

    // Alternate skip and read data
    EXPECT_EQ(0x47, parser.getBits(8));
    parser.getBits(8);
    EXPECT_EQ(0x63, parser.getBits(8));
    parser.getBits(8);
    EXPECT_EQ(0x00, parser.getBits(8));
    parser.getBits(8);
    EXPECT_EQ(0xb0, parser.getBits(8));
    parser.getBits(8);
}

/*!
 * Tests that we can skip bits less than 64 bits
 */
TEST_F(GetBitsTest, TestSkipBytesLessThan64Bits)
{
    EXPECT_EQ(sizeof(testData), 188);

    // Alternate skip and read data
    EXPECT_EQ(0x47, parser.getBits(8));
    parser.getBits(16);
    EXPECT_EQ(0x15, parser.getBits(8));
    parser.getBits(32);
    EXPECT_EQ(0x00, parser.getBits(8));
    parser.getBits(64);
    EXPECT_EQ(0x02, parser.getBits(8));
    EXPECT_EQ(0xe0, parser.getBits(8));
}

/*!
 * Tests that we can skip bits greater than 64 bits
 * and less than 64 bits
 */
TEST_F(GetBitsTest, test_skip_bits)
{
    EXPECT_EQ(sizeof(testData), 188);

    // Alternate skip and read data
    EXPECT_EQ(0x47, parser.getBits(8));
    parser.getBits(16);
    EXPECT_EQ(0x15, parser.getBits(8));
    parser.getBits(32);
    EXPECT_EQ(0x00, parser.getBits(8));
    parser.getBits(64);
    EXPECT_EQ(0x02, parser.getBits(8));
    EXPECT_EQ(0xe0, parser.getBits(8));
    parser.skipBits(72);
    EXPECT_EQ(0xf0, parser.getBits(8));
    EXPECT_EQ(0x09, parser.getBits(8));


    parser.skipBits(88);
    EXPECT_EQ(0x73, parser.getBits(8));
    EXPECT_EQ(0xf1, parser.getBits(8));


    parser.skipBits(128);
    EXPECT_EQ(0x1c, parser.getBits(8));
    EXPECT_EQ(0x01, parser.getBits(8));

    parser.skipBits(8);
    EXPECT_EQ(0xe7, parser.getBits(8));

    parser.skipBits(16);
    EXPECT_EQ(0xea, parser.getBits(8));
}

TEST_F(GetBitsTest, TestGetBitsMoreThan64)
{
    EXPECT_EQ(sizeof(testData), 188);

    try
    {
        parser.getBits(65);
        FAIL() << "Expected std::out_of_range";
    }
    catch (GetBitsException& err)
    {
        EXPECT_EQ(err.what(), std::string("Cannot parse more than 64 individual bits at a time."));
    }
}

/// Testing we throw exception when no data available
TEST_F(GetBitsTest, test_with_no_data_expect_failure)
{
    ExpectCorrectException([&] { empty_parser.getBits(8); }, GetBitsException("null input data"));
}

/// Testing we can only request maximum data size 64 bits
TEST_F(GetBitsTest, test_with_overlimit_data_expect_failure)
{
    ExpectCorrectException([&] { parser.getBits(65); },
                           GetBitsException(
                           "Cannot parse more than 64 individual bits at a time."));
}

/// Testing we can only request maximum data
TEST_F(GetBitsTest, test_with_out_of_bound_read_expect_failure)
{
    ExpectCorrectException(
    [&] {
        parser.resetBits(limitedData, sizeof(limitedData));

        EXPECT_EQ(0x47, parser.getBits(8));
        EXPECT_EQ(0x40, parser.getBits(8));
        EXPECT_EQ(0x63, parser.getBits(8));
        EXPECT_EQ(0x15, parser.getBits(8));
        EXPECT_EQ(0x00, parser.getBits(8));
        EXPECT_EQ(0x02, parser.getBits(8));
        EXPECT_EQ(0xb0, parser.getBits(8));
        EXPECT_EQ(0x5b, parser.getBits(8));
        EXPECT_EQ(0x00, parser.getBits(8));
        EXPECT_EQ(0x01, parser.getBits(8));
        parser.getBits(8); /// This doesnt exist in limitedData
    },
    GetBitsException("getBits: Out of bound read"));
}

/// Testing we can only skip maximum data
TEST_F(GetBitsTest, test_skip_bits_expect_failure)
{
    ExpectCorrectException(
    [&] {
        parser.resetBits(limitedData, sizeof(limitedData));

        parser.skipBits(65);
        parser.skipBits(8);
        parser.skipBits(8);
        parser.skipBits(8);
        parser.skipBits(8);
        parser.skipBits(8);
        parser.skipBits(8);
        parser.skipBits(8);
        parser.skipBits(8);
        parser.skipBits(8);
        parser.skipBits(8); /// This doesnt exist in limitedData
    },
    GetBitsException("getBits: Out of bound read"));
}

/// Testing we can only skip maximum data
TEST_F(GetBitsTest, test_skip_more_than_64_bits_expect_failure)
{
    ExpectCorrectException(
    [&] {
        parser.resetBits(limitedData, sizeof(limitedData));

        parser.skipBits(80);
        parser.skipBits(65); /// This doesnt exist in limitedData
    },
    GetBitsException("skipBits: Out of bound read"));
}

/// Testing we can only skip maximum data
TEST_F(GetBitsTest, test_skip_bytes_beyond_data_expect_failure)
{
    ExpectCorrectException(
    [&] {
        parser.resetBits(limitedData, sizeof(limitedData));

        parser.skipBytes(9);
        parser.skipBytes(1); // TODO Is this correct?
        // parser.skipBytes(65); /// This doesnt exist in limitedData
    },
    GetBitsException("getBits: Out of bound read mSrcInx: 9"));
}

/// Make coverage happy...
TEST_F(GetBitsTest, test_print_src_bytes)
{
    parser.printSrcBytes();
}
