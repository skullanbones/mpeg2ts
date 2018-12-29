/**
 * @class TsStatistics
 *
 * @brief Gather transport stream statistics
 *
 * @author skullanbones
 *
 * @version $Revision: 0.1 $
 *
 * @date $Date: 2018/02/23 00:16:20 $
 *
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */

/// 3rd-party
#include <plog/Log.h>

/// project files
#include "Logging.h"
#include "TsStatistics.h"
#include <public/mpeg2ts.h>

namespace mpeg2ts
{

// constants
const int64_t TsStatistics::CLOCK_90_KHZ = 90000;
const int64_t TsStatistics::TIME_STAMP_JUMP_DISCONTINUITY_LEVEL = 3 * CLOCK_90_KHZ;

TsStatistics::TsStatistics()
{
}

void TsStatistics::checkCCError(int a_pid, uint8_t a_cc)
{
    if (mPidStatistics[a_pid].lastCC == PidStatistic::INVALID_CC)
    {
        mPidStatistics[a_pid].lastCC = a_cc;
    }
    if (mPidStatistics[a_pid].lastCC != a_cc)
    {
        if (((mPidStatistics[a_pid].lastCC + 1) & 0x0f) != a_cc)
        {
            ++mPidStatistics[a_pid].numberOfCCErrors;
            LOGD_(FileLog) << "Continuity counter error at ts packet " << mCounters.mTsPacketCounter << " on pid "
                           << a_pid << ", last: " << static_cast<int>(mPidStatistics[a_pid].lastCC)
                           << ", cur: " << static_cast<int>(a_cc) << '\n';
        }
        mPidStatistics[a_pid].lastCC = a_cc;
    }
}

void TsStatistics::checkTsDiscontinuity(int a_pid, bool a_isDiscontinuous)
{
    if (a_isDiscontinuous)
    {
        ++mPidStatistics[a_pid].numberOfTsDiscontinuities;
        LOGD_(FileLog) << "Transport stream discontinuity at ts packet "
                       << mCounters.mTsPacketCounter << " on pid " << a_pid << '\n';
    }
}

void TsStatistics::buildPtsHistogram(int a_pid, int64_t a_pts)
{
    if (a_pts == -1)
    {
        mPidStatistics[a_pid].numberOfMissingPts++;
        return;
    }
    if (mPidStatistics[a_pid].lastPts == -1)
    {
        mPidStatistics[a_pid].lastPts = a_pts;
        return;
    }
    auto diff = a_pts - mPidStatistics[a_pid].lastPts;
    mPidStatistics[a_pid].ptsHistogram[diff]++;
    if (diff > TIME_STAMP_JUMP_DISCONTINUITY_LEVEL)
    {
        LOGD_(FileLog)
        << "PTS discontinuity at ts packet " << mCounters.mTsPacketCounter << " on pid " << a_pid
        << " pts-1 " << mPidStatistics[a_pid].lastPts << " pts-0 " << a_pts << " pts diff " << diff;
    }
    mPidStatistics[a_pid].lastPts = a_pts;
}

void TsStatistics::buildDtsHistogram(int a_pid, int64_t a_dts)
{
    if (a_dts == -1)
    {
        mPidStatistics[a_pid].numberOfMissingDts++;
        return;
    }
    if (mPidStatistics[a_pid].lastDts == -1)
    {
        mPidStatistics[a_pid].lastDts = a_dts;
        return;
    }
    auto diff = a_dts - mPidStatistics[a_pid].lastDts;
    mPidStatistics[a_pid].dtsHistogram[diff]++;
    if (diff > TIME_STAMP_JUMP_DISCONTINUITY_LEVEL)
    {
        LOGD_(FileLog) << "DTS discontinuity at ts packet " << mCounters.mTsPacketCounter
                       << " on pid " << a_pid << " dts-1 " << mPidStatistics[a_pid].lastDts << " dts-0 "
                       << a_dts << " dts diff " << diff << '\n';
    }

    mPidStatistics[a_pid].lastDts = a_dts;
}

void TsStatistics::buildPcrHistogram(int a_pid, int64_t a_pcr)
{
    if (a_pcr == -1)
    {
        return;
    }
    if (mPidStatistics[a_pid].lastPcr == -1)
    {
        mPidStatistics[a_pid].lastPcr = a_pcr;
        return;
    }
    auto diff = a_pcr - mPidStatistics[a_pid].lastPcr;
    mPidStatistics[a_pid].pcrHistogram[diff]++;
    if (diff > TIME_STAMP_JUMP_DISCONTINUITY_LEVEL * 300)
    {
        LOGD_(FileLog) << "PCR discontinuity at ts packet " << mCounters.mTsPacketCounter
                       << " on pid " << a_pid << " pcr-1 " << mPidStatistics[a_pid].lastPcr << " pcr-0 "
                       << a_pcr << " pcr diff " << diff << '\n';
    }

    mPidStatistics[a_pid].lastPcr = a_pcr;
}

} // namespace mpeg2ts
