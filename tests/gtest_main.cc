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