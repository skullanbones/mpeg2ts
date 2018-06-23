//
// Created by microlab on 6/23/18.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

/// Project files
#include "PsiTables.h"

TEST(PsiTablesTests, PatTableTestComparisonOperator)
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