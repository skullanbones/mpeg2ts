
#include <fstream>
#include <iostream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Project files
#include "TsPacketTestData.h"
#include "mpeg2ts.h"

// Class Under Test (CUT)
#include "TsUtilities.h"

using namespace mpeg2ts;
using namespace tsutil;

class TsUtilitiesTest : public ::testing::Test
{
public:
    void SetUp() override
    {
        createSettingsFile();
    }

    void TearDown() override
    {
        deleteSettingsFile();
    }

    void createSettingsFile()
    {
        std::ofstream outfile("settings.json");
        outfile << "\{\n"
                   "\"settings\": {\n"
                   "\"logLevel\": \"ERROR\",\n"
                   "\"logFileName\": \"mpeg2ts_log.csv\",\n"
                   "\"logFileMaxSize\": 102400,\n"
                   "\"logFileMaxNumberOf\": 10\n"
                   "}\n"
                   "}"
                << std::endl;
        outfile.close();
    }

    void deleteSettingsFile()
    {
        if (remove("settings.json") != 0)
            perror("Error deleting file");
        else
            puts("File settings.json successfully deleted");
    }

    TsUtilities m_tsUtil;
};

TEST_F(TsUtilitiesTest, test_parseTransportStreamData_1_success)
{
    EXPECT_TRUE(m_tsUtil.parseTransportStreamData(pat_packet_1, sizeof(pat_packet_1)));
    mpeg2ts::PatTable pat;
    pat = m_tsUtil.getPatTable();
    std::vector<uint16_t> pmtPids;
    pmtPids = m_tsUtil.getPmtPids();
    EXPECT_EQ(pmtPids.size(), 1);
    EXPECT_EQ(pmtPids.at(0), 598);
}

TEST_F(TsUtilitiesTest, test_parseTransportStreamData_1_fail)
{
    EXPECT_FALSE(m_tsUtil.parseTransportStreamData(NULL, 0));
}

TEST_F(TsUtilitiesTest, test_parseTransportStreamData_2_success)
{
    EXPECT_TRUE(m_tsUtil.parseTransportStreamData(pat_packet_2, sizeof(pat_packet_2)));
    mpeg2ts::PatTable pat;
    pat = m_tsUtil.getPatTable();
    const int kNumPmts = 17;
    EXPECT_EQ(pat.programs.size(), kNumPmts);
    EXPECT_EQ(pat.programs.at(0).program_map_PID, 16);
    // The first is a NIT table
    EXPECT_EQ(pat.programs.at(0).type, ProgramType::NIT);
    // The rest should be PMTs
    for (int i{ 1 }; i < kNumPmts; ++i)
    {
        EXPECT_EQ(pat.programs.at(i).type, ProgramType::PMT);
    }

    std::vector<uint16_t> pmtPids;
    pmtPids = m_tsUtil.getPmtPids();
    EXPECT_EQ(pmtPids.size(), kNumPmts - 1);
    EXPECT_EQ(pmtPids.at(0), 5180); // Check description in TsPacketTestData.h
    EXPECT_EQ(pmtPids.at(1), 5050);
    EXPECT_EQ(pmtPids.at(2), 5070);
    EXPECT_EQ(pmtPids.at(3), 5540);
    EXPECT_EQ(pmtPids.at(4), 1240);
    EXPECT_EQ(pmtPids.at(5), 5640);
    EXPECT_EQ(pmtPids.at(6), 1010);
    EXPECT_EQ(pmtPids.at(7), 5840);
    EXPECT_EQ(pmtPids.at(8), 5800);
    EXPECT_EQ(pmtPids.at(9), 1290);
    EXPECT_EQ(pmtPids.at(10), 1280);
    EXPECT_EQ(pmtPids.at(11), 870);
    EXPECT_EQ(pmtPids.at(12), 1810);
    EXPECT_EQ(pmtPids.at(13), 3010);
    EXPECT_EQ(pmtPids.at(14), 3020);
    EXPECT_EQ(pmtPids.at(15), 3030);
}

TEST_F(TsUtilitiesTest, test_getPmtTables_1_success)
{
    int totSize = sizeof(pat_packet_2) + sizeof(pmt_packet_1);
    printf("total size: %d\n", totSize);
    uint8_t buf[376]; // 188 * 2
    memcpy(buf, pat_packet_2, sizeof(pat_packet_2));
    memcpy(buf + sizeof(pat_packet_2), pmt_packet_1, sizeof(pmt_packet_1));

    EXPECT_TRUE(m_tsUtil.parseTransportStreamData(buf, totSize));
    mpeg2ts::PatTable pat;
    pat = m_tsUtil.getPatTable();
    const int kNumPmts = 17;
    EXPECT_EQ(pat.programs.size(), kNumPmts);

    std::vector<uint16_t> pmtPids;
    pmtPids = m_tsUtil.getPmtPids();
    EXPECT_EQ(pmtPids.size(), kNumPmts - 1);

    std::map<int, mpeg2ts::PmtTable> pmtTables;
    pmtTables = m_tsUtil.getPmtTables();

    EXPECT_EQ(pmtTables.size(), 1);
    EXPECT_EQ(pmtTables[1010].streams.size(), 4);
    EXPECT_EQ(pmtTables[1010].descriptors.size(), 0);

    // Verify streams PIDs
    EXPECT_EQ(pmtTables[1010].streams.at(0).elementary_PID, 1004);
    EXPECT_EQ(pmtTables[1010].streams.at(1).elementary_PID, 1018);
    EXPECT_EQ(pmtTables[1010].streams.at(2).elementary_PID, 1017);
    EXPECT_EQ(pmtTables[1010].streams.at(3).elementary_PID, 1019);

    // Test getEsPids
    std::vector<uint16_t> esPids;
    esPids = m_tsUtil.getEsPids();
    EXPECT_EQ(esPids.size(), 4);
    EXPECT_EQ(esPids.at(0), 1004);
    EXPECT_EQ(esPids.at(1), 1018);
    EXPECT_EQ(esPids.at(2), 1017);
    EXPECT_EQ(esPids.at(3), 1019);
}

TEST_F(TsUtilitiesTest, test_parseTransportFile_success)
{
    try
    {
        EXPECT_TRUE(m_tsUtil.parseTransportFile("../assets/bbc_one.ts"));
        mpeg2ts::PatTable pat;
        pat = m_tsUtil.getPatTable();
        std::vector<uint16_t> pmtPids;
        pmtPids = m_tsUtil.getPmtPids();
        EXPECT_EQ(pmtPids.size(), 1);
        EXPECT_EQ(pmtPids.at(0), 258);

        // Check PMT tables
        std::map<int, mpeg2ts::PmtTable> pmtTables;
        pmtTables = m_tsUtil.getPmtTables();
        EXPECT_EQ(pmtTables.size(), 1);
        EXPECT_EQ(pmtTables[258].streams.size(), 12);
        EXPECT_EQ(pmtTables[258].descriptors.size(), 1);

        // Verify streams PIDs
        EXPECT_EQ(pmtTables[258].streams.at(0).elementary_PID, 2304);
        EXPECT_EQ(pmtTables[258].streams.at(1).elementary_PID, 2306);
        EXPECT_EQ(pmtTables[258].streams.at(2).elementary_PID, 2342);
        EXPECT_EQ(pmtTables[258].streams.at(3).elementary_PID, 2305);

        EXPECT_EQ(pmtTables[258].streams.at(4).elementary_PID, 2307);
        EXPECT_EQ(pmtTables[258].streams.at(5).elementary_PID, 2308);
        EXPECT_EQ(pmtTables[258].streams.at(6).elementary_PID, 2309);
        EXPECT_EQ(pmtTables[258].streams.at(7).elementary_PID, 2310);

        EXPECT_EQ(pmtTables[258].streams.at(8).elementary_PID, 2311);
        EXPECT_EQ(pmtTables[258].streams.at(9).elementary_PID, 2312);
        EXPECT_EQ(pmtTables[258].streams.at(10).elementary_PID, 2313);
        EXPECT_EQ(pmtTables[258].streams.at(11).elementary_PID, 2314);

        std::vector<uint16_t> esPids;
        esPids = m_tsUtil.getEsPids();
        EXPECT_EQ(esPids.size(), 12);
        EXPECT_EQ(esPids.at(0), 2304);
        EXPECT_EQ(esPids.at(1), 2306);
        EXPECT_EQ(esPids.at(2), 2342);
        EXPECT_EQ(esPids.at(3), 2305);

        EXPECT_EQ(esPids.at(4), 2307);
        EXPECT_EQ(esPids.at(5), 2308);
        EXPECT_EQ(esPids.at(6), 2309);
        EXPECT_EQ(esPids.at(7), 2310);

        EXPECT_EQ(esPids.at(8), 2311);
        EXPECT_EQ(esPids.at(9), 2312);
        EXPECT_EQ(esPids.at(10), 2313);
        EXPECT_EQ(esPids.at(11), 2314);
    }
    catch (std::exception& e)
    {
        FAIL() << "Should not throw an exception!: " << e.what();
    }
    catch (...)
    {
        FAIL() << "Should not throw an exception!";
    }
}

TEST_F(TsUtilitiesTest, test_parseTransportFile_fail)
{
    EXPECT_FALSE(m_tsUtil.parseTransportFile("../assets/does_not_exist.ts"));
}

TEST_F(TsUtilitiesTest, test_parseTransportUdpStream_fail)
{
    IpAddress ip{ "192.168.1.1" };
    Port port{ "1234" };
    EXPECT_FALSE(m_tsUtil.parseTransportUdpStream(ip, port));
}

TEST_F(TsUtilitiesTest, test_VideoMediaInfo)
{
    try
    {
        EXPECT_TRUE(m_tsUtil.parseTransportFile("../assets/bbc_one.ts"));
        VideoMediaInfo mediaInfo = m_tsUtil.getVideoMediaInfo();
        EXPECT_EQ(mediaInfo.PID, 2304);
        EXPECT_EQ(mediaInfo.mediaType, MediaType::Video);
        EXPECT_EQ(mediaInfo.codec, VideoCodecType::MPEG2);
        // EXPECT_EQ(mediaInfo.width, 720);
        // EXPECT_EQ(mediaInfo.height, 576);
        // EXPECT_EQ(mediaInfo.frameRate, "25");
    }
    catch (std::exception& e)
    {
        FAIL() << "Should not throw an exception!: " << e.what();
    }
    catch (...)
    {
        FAIL() << "Should not throw an exception!";
    }
}
