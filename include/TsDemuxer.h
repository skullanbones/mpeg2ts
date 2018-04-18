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
    {
    }

    uint8_t lastCC;
    uint64_t numberOfCCErrors;

    uint64_t numberOfTsDiscontinuities;

    int64_t lastPts;
    std::map<int64_t, uint64_t> ptsHistogram;

    int64_t lastDts;
    std::map<int64_t, uint64_t> dtsHistogram;
};

class Statistics
{
public:
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
            return;
        }
        if (mPidStatistics[pid].lastPts == -1)
        {
            mPidStatistics[pid].lastPts = pts;
            return;
        }
        mPidStatistics[pid].ptsHistogram[pts - mPidStatistics[pid].lastPts]++;
        mPidStatistics[pid].lastPts = pts;
    }
    
    void buildDtsHistogram(int pid, int64_t dts)
    {
        if (dts == -1)
        {
            return;
        }
        if (mPidStatistics[pid].lastDts == -1)
        {
            mPidStatistics[pid].lastDts = dts;
            return;
        }
        mPidStatistics[pid].dtsHistogram[dts - mPidStatistics[pid].lastDts]++;
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
