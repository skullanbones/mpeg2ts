
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <iostream>

/// Project files
#include "TsPacketInfo.h"
#include "TsPacketTestData.h"
#include "TsDemuxer.h"


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

void PATCallback(PsiTable* table, uint16_t pid, void* hdl) {
    std::cout << "came here PATCallback" << std::endl;
    IDemuxerCallbacks* instance = reinterpret_cast<IDemuxerCallbacks*>(hdl);
    instance->onPatCallback();
}

void PMTCallback(PsiTable* table, uint16_t pid, void* hdl) {
    std::cout << "came here PMTCallback" << std::endl;
    IDemuxerCallbacks* instance = reinterpret_cast<IDemuxerCallbacks*>(hdl);
    instance->onPmtCallback();
}

/*!
 * Test we can demux a PAT packet
 */
TEST(TsDemuxerTests, TestDemuxPatPacket)
{
    try
    {
        TsDemuxer demuxer;
        std::shared_ptr<MockCallback> mcallback(new StrictMock<MockCallback>);

        demuxer.addPsiPid(TS_PACKET_PID_PAT, std::bind(&PATCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), mcallback.get());
        EXPECT_CALL((*mcallback.get()), onPatCallback()).Times(1);
        demuxer.demux(pat_packet_1);
    }
    catch(std::exception& e)
    {
        std::cout << "Got exception: " << e.what() << std::endl;
    }
}

/*!
 * Test we can demux 2 PAT packets
 */
TEST(TsDemuxerTests, TestDemux2PatPacket)
{
    try
    {
        TsDemuxer demuxer;
        std::shared_ptr<MockCallback> mcallback(new StrictMock<MockCallback>);

        demuxer.addPsiPid(TS_PACKET_PID_PAT, std::bind(&PATCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), mcallback.get());
        EXPECT_CALL((*mcallback.get()), onPatCallback()).Times(2);
        demuxer.demux(pat_packet_1);
        demuxer.demux(pat_packet_2);
    }
    catch(std::exception& e)
    {
        std::cout << "Got exception: " << e.what() << std::endl;
    }
}

/*!
 * Test we can demux a PMT packet
 */
TEST(TsDemuxerTests, TestDemuxPmtPacket)
{
    try
    {
        TsDemuxer demuxer;
        std::shared_ptr<MockCallback> mcallback(new StrictMock<MockCallback>);

        demuxer.addPsiPid(1010, std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), mcallback.get());
        EXPECT_CALL((*mcallback.get()), onPmtCallback()).Times(1);
        demuxer.demux(pmt_packet_1);
    }
    catch(std::exception& e)
    {
        std::cout << "Got exception: " << e.what() << std::endl;
    }
}

/*!
 * Test we can demux a big PMT packet
 */
TEST(TsDemuxerTests, TestDemuxServeralPmtPackets)
{
    try
    {
        TsDemuxer demuxer;
        std::shared_ptr<MockCallback> mcallback(new StrictMock<MockCallback>);

        demuxer.addPsiPid(50, std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), mcallback.get());
        EXPECT_CALL((*mcallback.get()), onPmtCallback()).Times(1);
        demuxer.demux(large_pmt_ts_packet_1);
        demuxer.demux(large_pmt_ts_packet_2);
        demuxer.demux(large_pmt_ts_packet_3);
    }
    catch(std::exception& e)
    {
        std::cout << "Got exception: " << e.what() << std::endl;
    }
}

/*!
 * Test we can demux a big PMT packet with alternating of other PAT tables
 */
TEST(TsDemuxerTests, TestDemuxServeralPmtPacketsAlternatingOtherPat)
{
    try
    {
        TsDemuxer demuxer;
        std::shared_ptr<MockCallback> mcallback(new StrictMock<MockCallback>);

        demuxer.addPsiPid(50, std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), mcallback.get());
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
    catch(std::exception& e)
    {
        std::cout << "Got exception: " << e.what() << std::endl;
    }
}

