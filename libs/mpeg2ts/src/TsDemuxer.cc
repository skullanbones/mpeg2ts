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
#include <cstdint>              // for uint8_t
#include <functional>           // for function
#include <map>                  // for map, _Rb_tree_iterator, map<>::mapped...
#include <memory>               // for unique_ptr

// 3rd-party
#include "plog/Log.h"           // for LOGE_

#include "Logging.h"            // for FileLog
#include "TsParser.h"           // for TsParser
#include "TsStatistics.h"       // for TsStatistics
#include "Ts_IEC13818-1.h"      // for PSI_TABLE_ID_PAT, PSI_TABLE_ID_PMT
#include "mpeg2ts.h"            // for TsDemuxer, TsPacketInfo, PsiCallBackFnc
#include "mpeg2ts_version.h"    // for getMpeg2tsVersion()


namespace mpeg2ts
{
TsDemuxer::TsDemuxer()
    : mParser{ std::unique_ptr<TsParser>(new TsParser()) }
{
}

TsDemuxer::~TsDemuxer()
{
}


void TsDemuxer::demux(const uint8_t* a_tsPacket)
{
    TsPacketInfo tsPacketInfo = {};
    mParser->parseTsPacketInfo(a_tsPacket, tsPacketInfo);

    if (tsPacketInfo.errorIndicator)
    {
        mParser->mStatistics.addTsPacketErrorIndicator();
    }

    if (tsPacketInfo.pid == TS_PACKET_PID_NULL)
    {
        mParser->mStatistics.addTsPacketNullPacketCounter();
        return; // Skip null packets, they contain no info
    }

    if (mTsCallbackMap.find(tsPacketInfo.pid) != mTsCallbackMap.end())
    {
        mTsCallbackMap[tsPacketInfo.pid](a_tsPacket, tsPacketInfo, mHandlers[tsPacketInfo.pid]);
    }

    if (mPsiCallbackMap.find(tsPacketInfo.pid) != mPsiCallbackMap.end())
    {
        // Check what table
        int table_id;
        mParser->collectTable(a_tsPacket, tsPacketInfo, table_id, mOrigin);

        if (table_id == PSI_TABLE_ID_PAT)
        {
            // Error check
            if (tsPacketInfo.pid != TS_PACKET_PID_PAT)
            {
                LOGE_(FileLog) << "ERROR: Stream does not conform to 13818-1 TS standard.";
            }
            else
            {
                PatTable pat = mParser->parsePatPacket(tsPacketInfo.pid);
                mPsiCallbackMap[tsPacketInfo.pid](mParser->getRawTable(tsPacketInfo.pid), &pat,
                                                  tsPacketInfo.pid, mHandlers[tsPacketInfo.pid]);
            }
        }
        else if (table_id == PSI_TABLE_ID_PMT)
        {

            PmtTable pmt = mParser->parsePmtPacket(tsPacketInfo.pid);
            mPsiCallbackMap[tsPacketInfo.pid](mParser->getRawTable(tsPacketInfo.pid), &pmt,
                                              tsPacketInfo.pid, mHandlers[tsPacketInfo.pid]);
        }
    }

    if (mPesCallbackMap.find(tsPacketInfo.pid) != mPesCallbackMap.end())
    {
        PesPacket pes;
        if (mParser->collectPes(a_tsPacket, tsPacketInfo, pes, mOrigin))
        {
            mPesCallbackMap[tsPacketInfo.pid](pes.mPesBuffer, pes, tsPacketInfo.pid,
                                              mHandlers[tsPacketInfo.pid]);
        }
    }

    mParser->mStatistics.addTsPacketCounter();
}

void TsDemuxer::addPsiPid(int a_pid, PsiCallBackFnc a_cb, void* a_hdl)
{
    mPsiCallbackMap[a_pid] = a_cb;
    mHandlers[a_pid] = a_hdl;
}

void TsDemuxer::addPesPid(int a_pid, PesCallBackFnc a_cb, void* a_hdl)
{
    mPesCallbackMap[a_pid] = a_cb;
    mHandlers[a_pid] = a_hdl;
}

void TsDemuxer::addTsPid(int a_pid, TsCallBackFnc a_cb, void* a_hdl)
{
    mTsCallbackMap[a_pid] = a_cb;
    mHandlers[a_pid] = a_hdl;
}

PidStatisticsMap TsDemuxer::getPidStatistics() const
{
    return mParser->mStatistics.getPidStatistics();
}

TsCounters TsDemuxer::getTsCounters() const
{
    return mParser->mStatistics.getTsCounters();
}

std::string TsDemuxer::getMpeg2tsLibVersion() const
{
    return getMpeg2tsVersion();
}

unsigned TsDemuxer::getMpeg2tsLibVersionMajor() const
{
    return getMpeg2tsVersionMajor();
}

unsigned TsDemuxer::getMpeg2tsLibVersionMinor() const
{
	return getMpeg2tsVersionMinor();
}

unsigned TsDemuxer::getMpeg2tsLibVersionPatch() const
{
	return getMpeg2tsVersionPatch();
}

unsigned TsDemuxer::getMpeg2tsLibVersionTweak() const
{
	return  getMpeg2tsVersionTweak();
}

void TsDemuxer::setOrigin(int64_t origin)
{
    mOrigin = origin;
}

int64_t TsDemuxer::getOrigin(int pid)
{
    return mParser->getOrigin(pid);
}

} // mpeg2ts
