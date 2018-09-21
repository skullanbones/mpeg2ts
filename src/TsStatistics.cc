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
#include "public/mpeg2ts.h"
#include "TsStatistics.h"
#include "Logging.h"

namespace mpeg2ts
{


TsStatistics::TsStatistics()
{
}

void TsStatistics::checkCCError(int pid, uint8_t cc)
{
    if (mPidStatistics[pid].lastCC == PidStatistic::INVALID_CC)
    {
        mPidStatistics[pid].lastCC = cc;
    }
    if (mPidStatistics[pid].lastCC != cc)
    {
        if (((mPidStatistics[pid].lastCC + 1) & 0x0f) != cc)
        {
            ++mPidStatistics[pid].numberOfCCErrors;
            LOGD_(FileLog) << "Continuity counter error at ts packet " << mCounters.mTsPacketCounter << " on pid "
                << pid << "\n";
        }
        mPidStatistics[pid].lastCC = cc;
    }
}

void TsStatistics::checkTsDiscontinuity(int pid, bool isDiscontinuous)
{
    if (isDiscontinuous)
    {
        ++mPidStatistics[pid].numberOfTsDiscontinuities;
        LOGD_(FileLog) << "Transport stream discontinuity at ts packet " << mCounters.mTsPacketCounter
            << " on pid " << pid << "\n";
    }
}

void TsStatistics::buildPtsHistogram(int pid, int64_t pts)
{
    if (pts == -1)
    {
        mPidStatistics[pid].numberOfMissingPts++;
        return;
    }
    if (mPidStatistics[pid].lastPts == -1)
    {
        mPidStatistics[pid].lastPts = pts;
        return;
    }
    auto diff = pts - mPidStatistics[pid].lastPts;
    mPidStatistics[pid].ptsHistogram[diff]++;
    if (diff > TIME_STAMP_JUMP_DISCONTINUITY_LEVEL)
    {
        LOGD_(FileLog) << "PTS discontinuity at ts packet " << mCounters.mTsPacketCounter << " on pid " << pid << " pts-1 "
            << mPidStatistics[pid].lastPts << " pts-0 " << pts << " pts diff " << diff;
    }
    mPidStatistics[pid].lastPts = pts;
}

void TsStatistics::buildDtsHistogram(int pid, int64_t dts)
{
    if (dts == -1)
    {
        mPidStatistics[pid].numberOfMissingDts++;
        return;
    }
    if (mPidStatistics[pid].lastDts == -1)
    {
        mPidStatistics[pid].lastDts = dts;
        return;
    }
    auto diff = dts - mPidStatistics[pid].lastDts;
    mPidStatistics[pid].dtsHistogram[diff]++;
    if (diff > TIME_STAMP_JUMP_DISCONTINUITY_LEVEL)
    {
        LOGD_(FileLog) << "DTS discontinuity at ts packet " << mCounters.mTsPacketCounter << " on pid " << pid << " dts-1 "
            << mPidStatistics[pid].lastDts << " dts-0 " << dts << " dts diff " << diff << "\n";
    }

    mPidStatistics[pid].lastDts = dts;
}

void TsStatistics::buildPcrHistogram(int pid, int64_t pcr)
{
    if (pcr == -1)
    {
        return;
    }
    if (mPidStatistics[pid].lastPcr == -1)
    {
        mPidStatistics[pid].lastPcr = pcr;
        return;
    }
    auto diff = pcr - mPidStatistics[pid].lastPcr;
    mPidStatistics[pid].pcrHistogram[diff]++;
    if (diff > TIME_STAMP_JUMP_DISCONTINUITY_LEVEL * 300)
    {
        LOGD_(FileLog) << "PCR discontinuity at ts packet " << mCounters.mTsPacketCounter << " on pid " << pid << " pcr-1 "
            << mPidStatistics[pid].lastPcr << " pcr-0 " << pcr << " pcr diff " << diff << "\n";
    }

    mPidStatistics[pid].lastPcr = pcr;
}

} // namespace mpeg2ts
