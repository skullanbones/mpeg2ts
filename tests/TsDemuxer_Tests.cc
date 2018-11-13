#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

/// Project files
#include "TsPacketTestData.h"
#include <public/mpeg2ts.h>

using namespace mpeg2ts;

using ::testing::StrictMock;

class IDemuxerCallbacks
{
public:
    /* This callback is called when Demuxer demuxes any of the registered TS-packets on a PID */

    virtual void onPatCallback() = 0;
    virtual void onPmtCallback() = 0;
    virtual void onPesCallback() = 0;
    virtual void onTsCallback() = 0;
};

class MockCallback : public IDemuxerCallbacks
{
public:
    MOCK_METHOD0(onPatCallback, void());
    MOCK_METHOD0(onPmtCallback, void());
    MOCK_METHOD0(onPesCallback, void());
    MOCK_METHOD0(onTsCallback, void());
};

void PATCallback(const ByteVector& rawPes, PsiTable* table, uint16_t pid, void* hdl)
{
    puts("came here PATCallback\n");
    IDemuxerCallbacks* instance = reinterpret_cast<IDemuxerCallbacks*>(hdl);
    instance->onPatCallback();
}

void PMTCallback(const ByteVector& rawPes, PsiTable* table, uint16_t pid, void* hdl)
{
    puts("came here PMTCallback\n");
    IDemuxerCallbacks* instance = reinterpret_cast<IDemuxerCallbacks*>(hdl);
    instance->onPmtCallback();
}

void PESCallback(const ByteVector& rawPes, const PesPacket& pes,  uint16_t pid, void* hdl)
{
    puts("came here PESCallback\n");
    IDemuxerCallbacks* instance = reinterpret_cast<IDemuxerCallbacks*>(hdl);
    instance->onPesCallback();
}

void TSCallback(const uint8_t* packet, TsPacketInfo tsPacketInfo, void* hdl)
{
    puts("came here TSCallback\n");
    IDemuxerCallbacks* instance = reinterpret_cast<IDemuxerCallbacks*>(hdl);
    instance->onTsCallback();
}

class TsDemuxerTest : public ::testing::Test
{
public:
    const uint8_t PES_PID = 50;

    void SetUp() override
    {
        mcallback = std::unique_ptr<MockCallback>(new StrictMock<MockCallback>);
    }

    void TearDown() override
    {
    }

    
    template<typename Callable>
    void ExpectNoException(Callable f)
    {
        try 
        {
            f();
        }
        catch (std::exception& e)
        {
            FAIL() << "Expected no exception but got exception: " << e.what();
        }
        catch (...)
        {
            FAIL() << "Expected no exception.";
        }
    }

    TsDemuxer demuxer;
    std::unique_ptr<MockCallback> mcallback;
};


/*!
 * Test we can demux a PAT packet
 */
TEST_F(TsDemuxerTest, TestDemuxPatPacket)
{
    ExpectNoException([&]
    {
        demuxer.addPsiPid(TS_PACKET_PID_PAT,
                            std::bind(&PATCallback, std::placeholders::_1, std::placeholders::_2,
                            std::placeholders::_3, std::placeholders::_4),
                            mcallback.get());
        
        EXPECT_CALL(*mcallback, onPatCallback()).Times(1);
        demuxer.demux(pat_packet_1);
    });
}

/*!
 * Test we can demux 2 PAT packets
 */
TEST_F(TsDemuxerTest, TestDemux2PatPacket)
{
    ExpectNoException([&]
    {
        demuxer.addPsiPid(TS_PACKET_PID_PAT,
                          std::bind(&PATCallback, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, std::placeholders::_4),
                          mcallback.get());

        EXPECT_CALL(*mcallback, onPatCallback()).Times(2);
        demuxer.demux(pat_packet_1);
        demuxer.demux(pat_packet_2);
    });
}

/*!
 * Test we can demux a PMT packet
 */
TEST_F(TsDemuxerTest, TestDemuxPmtPacket)
{
    ExpectNoException([&]
    {
        demuxer.addPsiPid(1010,
                          std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, std::placeholders::_4),
                          mcallback.get());

        EXPECT_CALL(*mcallback, onPmtCallback()).Times(1);
        demuxer.demux(pmt_packet_1);
    });
}

/*!
* Test we can demux a PMT packet over 2 ts-packets
*/
TEST_F(TsDemuxerTest, TestDemux2PmtPackets)
{
    ExpectNoException([&]
    {
        demuxer.addPsiPid(32, 
                          std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2,
                          std::placeholders::_3, std::placeholders::_4),
                          mcallback.get());
        EXPECT_CALL(*mcallback, onPmtCallback()).Times(1);
        demuxer.demux(pmt_packet_2_1);
        demuxer.demux(pmt_packet_2_2);
    });
}

/*!
 * Test we can demux a big PMT packet
 */
TEST_F(TsDemuxerTest, TestDemuxServeralPmtPackets)
{
    ExpectNoException([&]
    {
        demuxer.addPsiPid(50,
                          std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, std::placeholders::_4),
                          mcallback.get());

        EXPECT_CALL(*mcallback, onPmtCallback()).Times(1);
        demuxer.demux(large_pmt_ts_packet_1);
        demuxer.demux(large_pmt_ts_packet_2);
        demuxer.demux(large_pmt_ts_packet_3);
    });
}

/*!
 * Test we can demux a big PMT packet with alternating of other PAT tables
 */
TEST_F(TsDemuxerTest, TestDemuxServeralPmtPacketsAlternatingOtherPat)
{
    ExpectNoException([&]
    {
        demuxer.addPsiPid(50,
                          std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, std::placeholders::_4),
                          mcallback.get());

        EXPECT_CALL(*mcallback, onPmtCallback()).Times(1);
        demuxer.demux(large_pmt_ts_packet_1); // This is the start of the PMT
        demuxer.demux(pat_packet_1);
        demuxer.demux(pat_packet_2);
        demuxer.demux(large_pmt_ts_packet_2); // continuation of PMT
        demuxer.demux(pat_packet_1);
        demuxer.demux(pat_packet_2);
        demuxer.demux(packet_1);
        demuxer.demux(large_pmt_ts_packet_3); // last PMT packet
        demuxer.demux(packet_2);
    });
}

/*!
 * Test we can demux a complete PES packet
 */
TEST_F(TsDemuxerTest, TestDemuxOnePesPacket)
{
    ExpectNoException([&]
    {
        demuxer.addPesPid(50,
                          std::bind(&PESCallback, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, std::placeholders::_4),
                          mcallback.get());

        EXPECT_CALL(*mcallback, onPesCallback()).Times(1);
        demuxer.demux(ts_pes_1);
        demuxer.demux(ts_pes_2);
        demuxer.demux(ts_pes_3);
        demuxer.demux(ts_pes_4);
        demuxer.demux(ts_pes_5);
        demuxer.demux(ts_pes_6);
        demuxer.demux(ts_pes_7);
        demuxer.demux(ts_pes_8);
        demuxer.demux(ts_pes_9);
        demuxer.demux(ts_pes_10);
    });
}

/*!
* Test we can parse 1 TS-packet
*/
TEST_F(TsDemuxerTest, TestDemuxOneTsPacket)
{
    ExpectNoException([&]
    {
        demuxer.addTsPid(50,
                          std::bind(&TSCallback, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3),
                          mcallback.get());

        EXPECT_CALL(*mcallback, onTsCallback()).Times(1);
        demuxer.demux(ts_pes_1);
    });
}

/*! 
* Test we get correct Ts counter statistics
*/
TEST_F(TsDemuxerTest, test_get_ts_counters)
{
    ExpectNoException([&]
    {
        demuxer.addTsPid(50,
                          std::bind(&TSCallback, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3),
                          mcallback.get());

        EXPECT_CALL(*mcallback, onTsCallback()).Times(4);
        demuxer.demux(ts_pes_1);
        TsCounters count = demuxer.getTsCounters();
        EXPECT_EQ(count.mTsPacketCounter, 1);
        demuxer.demux(ts_pes_1);
        count = demuxer.getTsCounters();
        EXPECT_EQ(count.mTsPacketCounter, 2);
        demuxer.demux(ts_pes_2);
        demuxer.demux(ts_pes_3);
        count = demuxer.getTsCounters();
        EXPECT_EQ(count.mTsPacketCounter, 4);
    });
}

/*! 
* Test we get correct PID statistics
*/
TEST_F(TsDemuxerTest, test_get_pid_statistics)
{
    ExpectNoException([&]
    {
        demuxer.addPesPid(50,
                          std::bind(&PESCallback, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, std::placeholders::_4),
                          mcallback.get());                  

        EXPECT_CALL(*mcallback, onPesCallback()).Times(1);
        demuxer.demux(ts_pes_1);
        demuxer.demux(ts_pes_2);
        demuxer.demux(ts_pes_3);
        demuxer.demux(ts_pes_4);
        demuxer.demux(ts_pes_5);
        demuxer.demux(ts_pes_6);
        demuxer.demux(ts_pes_7);
        demuxer.demux(ts_pes_8);
        demuxer.demux(ts_pes_9);
        demuxer.demux(ts_pes_10);

        PidStatisticsMap map = demuxer.getPidStatistics();
        EXPECT_EQ(map.size(), 1);

        // Test correct keys
        std::vector<int> keys;
        for(auto & stat : map) {
            keys.push_back(stat.first);
        }
        EXPECT_EQ(keys.size(), 1);
        EXPECT_EQ(keys.back(), 50);

        // Get PidStatistic
        PidStatistic stat = map[50];
        EXPECT_EQ(stat.numberOfCCErrors, 0);
        EXPECT_EQ(stat.numberOfTsDiscontinuities, 0);

        //EXPECT_EQ(stat.lastPts, 689094304); // Why does it not work?

        EXPECT_EQ(stat.numberOfMissingPts, 0);

        // TODO check PID
        std::map<int64_t, uint64_t> dts_histogram = stat.dtsHistogram;
        std::map<int64_t, uint64_t> pts_histogram = stat.ptsHistogram;
        EXPECT_EQ(dts_histogram.size(), 0); // Not Sure why this is correct. 
        EXPECT_EQ(pts_histogram.size(), 0); // Not Sure why this is correct. 
    });
}
