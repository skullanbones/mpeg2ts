/*****************************************************************
*
*    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
*
*    mpeg2ts - mpeg2ts tests
*
*    This file is part of mpeg2ts (Mpeg2 Transport Stream Library).
*
*    Unless you have obtained mpeg2ts under a different license,
*    this version of mpeg2ts is mpeg2ts|GPL.
*    Mpeg2ts|GPL is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License as
*    published by the Free Software Foundation; either version 2,
*    or (at your option) any later version.
*
*    Mpeg2ts|GPL is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with mpeg2ts|GPL; see the file COPYING.  If not, write to the
*    Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
*    02111-1307, USA.
*
********************************************************************/
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Global asset
std::string asset_file;

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
 
    if (argc != 2)
    {
        std::cout << "You must use asset input for unit tests, example:\n";
        std::cout << "./run_gtests ~/Videos/bbc_one.ts\n";
        return 0;
    }

    asset_file = argv[1];

    if (asset_file.find("bbc_one.ts") == std::string::npos)
    {
        std::cout << "Must be asset bbc_one.ts!" << '\n';
        return 0;    
    }

    std::cout << "asset_file: " << asset_file << '\n';
 
    return RUN_ALL_TESTS();
    // system("PAUSE");
    // return 0;
}