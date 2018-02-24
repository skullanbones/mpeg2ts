//
// Created by microlab on 2/24/18.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

/// Project files
#include "TsParser.h"

TEST(TsParserConstants, CheckConstants)
{
    EXPECT_EQ(TS_PACKET_HEADER_SIZE, 4);
}

TEST(MathTest, TwoPlusTwoEqualsFour) {
    EXPECT_EQ(2 + 2, 4);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
