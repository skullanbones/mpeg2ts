/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */

#pragma once
#include <functional>
#include <iostream>
#include <map>

/// project files
#include "TsParser.h"
#include "TsStandards.h"

struct PidStatistic
{
    PidStatistic()
    : lastCC{ 16 }
    , numberOfCCErrors{0}
    , numberOfTsDiscontinuities{0}
    , lastPts{ -1 }
    , lastDts{ -1 }
    , numberOfMissingPts{0}
    , numberOfMissingDts{0}
    {
    }

    uint8_t lastCC;
    uint64_t numberOfCCErrors;

    uint64_t numberOfTsDiscontinuities;

    int64_t lastPts;
    std::map<int64_t, uint64_t> ptsHistogram;

    int64_t lastDts;
    std::map<int64_t, uint64_t> dtsHistogram;

    uint64_t numberOfMissingPts;
    
    uint64_t numberOfMissingDts;
};

class Statistics
{
public:
    static const int64_t TimestampJumpDiscontinuityLevel = 3 * 90000;// 3s
    Statistics()
        :mTsPacketCounter{0}
    {
    }
    
    void checkCCError(int pid, uint8_t cc)
    {
        if (mPidStatistics[pid].lastCC == 16)
        {
            mPidStatistics[pid].lastCC = cc;
        }
        if (mPidStatistics[pid].lastCC != cc)
        {
            if (((mPidStatistics[pid].lastCC + 1) & 0x0f) != cc)
            {
                std::cout << "Continuity counter error at ts packet " << mTsPacketCounter
                          << " on pid " << pid << "\n";
            }   
            mPidStatistics[pid].lastCC = cc;
        }
    }
    
    void checkTsDiscontinuity(int pid, bool dis)
    {
        if (dis)
        {
            ++mPidStatistics[pid].numberOfTsDiscontinuities;
            std::cout << "Transport stream discontinuity at ts packet " << mTsPacketCounter
                      << " on pid " << pid << "\n";
        }
    }

    void buildPtsHistogram(int pid, int64_t pts)
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
        if (diff > TimestampJumpDiscontinuityLevel)
        {
        std::cout << "PTS discontinuity at ts packet " << mTsPacketCounter
                      << " on pid " << pid << " pts-1 " << mPidStatistics[pid].lastPts << " pts-0 " << pts << " pts diff " << diff << "\n";
        }
        mPidStatistics[pid].lastPts = pts;
    }
    
    void buildDtsHistogram(int pid, int64_t dts)
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
        if (diff > TimestampJumpDiscontinuityLevel)
        {
        std::cout << "DTS discontinuity at ts packet " << mTsPacketCounter
                      << " on pid " << pid << " dts-1 " << mPidStatistics[pid].lastDts << " dts-0 " << dts << " dts diff " << diff << "\n";
        }

        mPidStatistics[pid].lastDts = dts;
    }

    std::map<int, PidStatistic> mPidStatistics;
    uint64_t mTsPacketCounter;
};

/// @brief Demux ts packets into PSI and PES (plus TS pass through)
class TsDemuxer : public Statistics
{
public:
    typedef std::function<void(PsiTable* table)> PsiCallBackFnc;
    typedef std::function<void(const PesPacket& table, uint16_t pid)> PesCallBackFnc;
    typedef std::function<void(const TsHeader& hdr)> TsCallBackFnc;

    TsDemuxer() = default;

    void demux(const uint8_t* tsPacket);

    void addPsiPid(int pid, PsiCallBackFnc cb);
    void addPesPid(int pid, PesCallBackFnc cb);
    void addTsPid(int pid, TsCallBackFnc cb);

protected:
    std::map<int, PsiCallBackFnc> mPsiCallbackMap; // TODO: make cb generic
    std::map<int, PesCallBackFnc> mPesCallbackMap;
private:
    TsParser mParser;
};
