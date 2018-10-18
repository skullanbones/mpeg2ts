
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
};

class MockCallback : public IDemuxerCallbacks
{
public:
    MOCK_METHOD0(onPatCallback, void());
    MOCK_METHOD0(onPmtCallback, void());
};

void PATCallback(const ByteVector& rawPes, PsiTable* table, uint16_t pid, void* hdl)
{
    std::cout << "came here PATCallback" << std::endl;
    IDemuxerCallbacks* instance = reinterpret_cast<IDemuxerCallbacks*>(hdl);
    instance->onPatCallback();
}

void PMTCallback(const ByteVector& rawPes, PsiTable* table, uint16_t pid, void* hdl)
{
    std::cout << "came here PMTCallback" << std::endl;
    IDemuxerCallbacks* instance = reinterpret_cast<IDemuxerCallbacks*>(hdl);
    instance->onPmtCallback();
}

class TsDemuxerTest : public ::testing::Test
{
public:
    void SetUp() override
    {
        mcallback = std::unique_ptr<MockCallback>(new StrictMock<MockCallback>);
    }

    void TearDown() override
    {
    }

    TsDemuxer demuxer;
    std::unique_ptr<MockCallback> mcallback;
};

/*!
 * Test we can demux a PAT packet
 */
TEST_F(TsDemuxerTest, TestDemuxPatPacket)
{
    try
    {
        demuxer.addPsiPid(TS_PACKET_PID_PAT,
                            std::bind(&PATCallback, std::placeholders::_1, std::placeholders::_2,
                            std::placeholders::_3, std::placeholders::_4),
                            mcallback.get());
        EXPECT_CALL((*mcallback.get()), onPatCallback()).Times(1);
        demuxer.demux(pat_packet_1);
    }
    catch (std::exception& e)
    {
        std::cout << "Got exception: " << e.what() << std::endl;
    }
}

/*!
 * Test we can demux 2 PAT packets
 */
TEST_F(TsDemuxerTest, TestDemux2PatPacket)
{
    try
    {
<<<<<<< HEAD
        TsDemuxer demuxer;
        std::shared_ptr<MockCallback> mcallback(new StrictMock<MockCallback>);

        demuxer.addPsiPid(TS_PACKET_PID_PAT,
                          std::bind(&PATCallback, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, std::placeholders::_4),
                          mcallback.get());
=======
        demuxer.addPsiPid(TS_PACKET_PID_PAT, std::bind(&PATCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), mcallback.get());
>>>>>>> Add fixture to TsDemuxer tests
        EXPECT_CALL((*mcallback.get()), onPatCallback()).Times(2);
        demuxer.demux(pat_packet_1);
        demuxer.demux(pat_packet_2);
    }
    catch (std::exception& e)
    {
        std::cout << "Got exception: " << e.what() << std::endl;
    }
}

/*!
 * Test we can demux a PMT packet
 */
TEST_F(TsDemuxerTest, TestDemuxPmtPacket)
{
    try
    {
<<<<<<< HEAD
        TsDemuxer demuxer;
        std::shared_ptr<MockCallback> mcallback(new StrictMock<MockCallback>);

        demuxer.addPsiPid(1010,
                          std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, std::placeholders::_4),
                          mcallback.get());
=======
        demuxer.addPsiPid(1010, std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), mcallback.get());
>>>>>>> Add fixture to TsDemuxer tests
        EXPECT_CALL((*mcallback.get()), onPmtCallback()).Times(1);
        demuxer.demux(pmt_packet_1);
    }
    catch (std::exception& e)
    {
        std::cout << "Got exception: " << e.what() << std::endl;
    }
}

/*!
 * Test we can demux a big PMT packet
 */
TEST_F(TsDemuxerTest, TestDemuxServeralPmtPackets)
{
    try
    {
<<<<<<< HEAD
        TsDemuxer demuxer;
        std::shared_ptr<MockCallback> mcallback(new StrictMock<MockCallback>);

        demuxer.addPsiPid(50,
                          std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, std::placeholders::_4),
                          mcallback.get());
=======
        demuxer.addPsiPid(50, std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), mcallback.get());
>>>>>>> Add fixture to TsDemuxer tests
        EXPECT_CALL((*mcallback.get()), onPmtCallback()).Times(1);
        demuxer.demux(large_pmt_ts_packet_1);
        demuxer.demux(large_pmt_ts_packet_2);
        demuxer.demux(large_pmt_ts_packet_3);
    }
    catch (std::exception& e)
    {
        std::cout << "Got exception: " << e.what() << std::endl;
    }
}

/*!
 * Test we can demux a big PMT packet with alternating of other PAT tables
 */
TEST_F(TsDemuxerTest, TestDemuxServeralPmtPacketsAlternatingOtherPat)
{
    try
    {
<<<<<<< HEAD
        TsDemuxer demuxer;
        std::shared_ptr<MockCallback> mcallback(new StrictMock<MockCallback>);

        demuxer.addPsiPid(50,
                          std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, std::placeholders::_4),
                          mcallback.get());
=======
        demuxer.addPsiPid(50, std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), mcallback.get());
>>>>>>> Add fixture to TsDemuxer tests
        EXPECT_CALL((*mcallback.get()), onPmtCallback()).Times(1);
        demuxer.demux(large_pmt_ts_packet_1); // This is the start of the PMT
        demuxer.demux(pat_packet_1);
        demuxer.demux(pat_packet_2);
        demuxer.demux(large_pmt_ts_packet_2); // continuation of PMT
        demuxer.demux(pat_packet_1);
        demuxer.demux(pat_packet_2);
        demuxer.demux(packet_1);
        demuxer.demux(large_pmt_ts_packet_3); // last PMT packet
        demuxer.demux(packet_2);
    }
    catch (std::exception& e)
    {
        std::cout << "Got exception: " << e.what() << std::endl;
    }
}
