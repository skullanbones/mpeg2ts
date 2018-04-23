/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */

#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <stdio.h>

struct PidStatistic
{
    static const uint8_t INVALID_CC = 16;
    PidStatistic()
    : lastCC{ INVALID_CC }
    , numberOfCCErrors{ 0 }
    , numberOfTsDiscontinuities{ 0 }
    , lastPts{ -1 }
    , lastDts{ -1 }
    , numberOfMissingPts{ 0 }
    , numberOfMissingDts{ 0 }
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

class TsStatistics
{
public:
    static const int64_t CLOCK_90_KHZ = 90000;
    static const int64_t TIME_STAMP_JUMP_DISCONTINUITY_LEVEL = 3 * CLOCK_90_KHZ; // 3s
    TsStatistics()
    : mTsPacketCounter{ 0 }
    {
    }

    void checkCCError(int pid, uint8_t cc);

    void checkTsDiscontinuity(int pid, bool dis);

    void buildPtsHistogram(int pid, int64_t pts);

    void buildDtsHistogram(int pid, int64_t dts);

    std::map<int, PidStatistic> mPidStatistics;
    uint64_t mTsPacketCounter;
};
