/*****************************************************************
*
*    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
*
*    mpeg2ts - mpeg2ts lib
*
*    This file is part of mpeg2ts (Mpeg2 Transport Stream Library).
*
*    Unless you have obtained mpeg2ts under a different license,
*    this version of mpeg2ts is mpeg2ts|GPL.
*    Mpeg2ts|GPL is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License as
*    published by the Free Software Foundation; either version 2,
*    or (at your option) any later version.
*
*    Mpeg2ts|GPL is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with mpeg2ts|GPL; see the file COPYING.  If not, write to the
*    Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
*    02111-1307, USA.
*
********************************************************************/
#ifndef _TSSTATISTICS_H
#define _TSSTATISTICS_H

#include <cstdint>              // for int64_t, for uint8_t

#include "mpeg2ts.h"            // for TsCounters, PidStatisticsMap

namespace mpeg2ts
{
class TsStatistics
{
public:
    // Constants
    static const int64_t CLOCK_90_KHZ;
    static const int64_t TIME_STAMP_JUMP_DISCONTINUITY_LEVEL; // 3s

    explicit TsStatistics();

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

    PidStatisticsMap getPidStatistics() const;
    TsCounters getTsCounters() const;

    void addTsPacketErrorIndicator();
    void addTsPacketNullPacketCounter();
    void addTsPacketCounter();

private:
    PidStatisticsMap mPidStatistics;
    TsCounters mCounters;
};

inline PidStatisticsMap TsStatistics::getPidStatistics() const
{
    return mPidStatistics;
}

inline TsCounters TsStatistics::getTsCounters() const
{
    return mCounters;
}

inline void TsStatistics::addTsPacketErrorIndicator()
{
    ++mCounters.mTsPacketErrorIndicator;
}

inline void TsStatistics::addTsPacketNullPacketCounter()
{
    ++mCounters.mTsPacketNullPacketCounter;
}

inline void TsStatistics::addTsPacketCounter()
{
    ++mCounters.mTsPacketCounter;
}

} // namespace mpeg2ts

#endif /* _TSSTATISTICS_H */
