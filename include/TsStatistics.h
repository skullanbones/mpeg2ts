/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */

#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <stdio.h>

const int64_t CLOCK_90_KHZ = 90000;
const int64_t TIME_STAMP_JUMP_DISCONTINUITY_LEVEL = 3 * CLOCK_90_KHZ; // 3s

struct PidStatistic
{
    static const uint8_t INVALID_CC = 16;
    PidStatistic()
    : lastCC{ INVALID_CC }
    , numberOfCCErrors{ 0 }
    , numberOfTsDiscontinuities{ 0 }
    , lastPts{ -1 }
    , lastDts{ -1 }
    , lastPcr{ -1 }
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

    int64_t lastPcr;
    std::map<int64_t, uint64_t> pcrHistogram;

    uint64_t numberOfMissingPts;

    uint64_t numberOfMissingDts;
};

class TsStatistics
{
public:
    TsStatistics()
    : mTsPacketCounter{ 0 }
    {
    }

    /*!
     * Calculates Continuity errors.
     * @param pid Filtered PID.
     * @param cc Current TS packets Continuity Counter.
     */
    void checkCCError(int pid, uint8_t cc);

    /*!
     * Book keep flagged TS packets discontinuities.
     * @param pid Filtered PID.
     * @param isDiscontinuous Whether or not this is a discontinuity.
     */
    void checkTsDiscontinuity(int pid, bool isDiscontinuous);

    /*!
     * Build a histogram of PTS differences between 2 time samples.
     * @param pid Filtered PID.
     * @param pts Program Time Stamp value.
     */
    void buildPtsHistogram(int pid, int64_t pts);

    /*!
    * Build a histogram of DTS differences between 2 time samples.
    * @param pid Filtered PID.
    * @param pts Display Time Stamp value.
    */
    void buildDtsHistogram(int pid, int64_t dts);

    /*!
    * Build a histogram of PCR differences between 2 time samples.
    * @param pid Filtered PID.
    * @param pts Program Clock Reference value.
    */
    void buildPcrHistogram(int pid, int64_t pcr);

    std::map<int, PidStatistic> mPidStatistics;
    uint64_t mTsPacketCounter;
};
