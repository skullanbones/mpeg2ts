/**
 * @class TsDemuxer
 *
 * @brief Demux transport stream packet
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
#include "TsDemuxer.h"
#include "GetBits.h"
#include <cstdint>


void TsDemuxer::demux(const uint8_t* tsPacket)
{
    TsPacketInfo tsPacketInfo = {};
    mParser.parseTsPacketInfo(tsPacket, tsPacketInfo);

    if (tsPacketInfo.errorIndicator)
    {
        ++mParser.mTsPacketErrorIndicator;
    }

    if (tsPacketInfo.pid == TS_PACKET_PID_NULL)
    {
        ++mParser.mTsPacketNullPacketCounter;
        return; // Skip null packets, they contain no info
    }

    if (mTsCallbackMap.find(tsPacketInfo.pid) != mTsCallbackMap.end())
    {
        mTsCallbackMap[tsPacketInfo.pid](tsPacket, tsPacketInfo, mHandlers[tsPacketInfo.pid]);
    }

    if (mPsiCallbackMap.find(tsPacketInfo.pid) != mPsiCallbackMap.end())
    {
        // TODO Filter PID from PSI, TS, PES etc...
        // Check what table
        uint8_t table_id;
        mParser.collectTable(tsPacket, tsPacketInfo, table_id);

        if (table_id == PSI_TABLE_ID_PAT)
        {
            PatTable pat = mParser.parsePatPacket();
            mPsiCallbackMap[tsPacketInfo.pid](&pat, mHandlers[tsPacketInfo.pid]);
        }
        else if (table_id == PSI_TABLE_ID_PMT)
        {
            PmtTable pmt = mParser.parsePmtPacket();
            mPsiCallbackMap[tsPacketInfo.pid](&pmt, mHandlers[tsPacketInfo.pid]);
        }
    }

    if (mPesCallbackMap.find(tsPacketInfo.pid) != mPesCallbackMap.end())
    {
        PesPacket pes;
        if (mParser.collectPes(tsPacket, tsPacketInfo, pes))
        {
            mPesCallbackMap[tsPacketInfo.pid](pes, tsPacketInfo.pid, mHandlers[tsPacketInfo.pid]);
        }
    }

    ++mParser.mTsPacketCounter;
}

void TsDemuxer::addPsiPid(int pid, PsiCallBackFnc cb, void* hdl)
{
    mPsiCallbackMap[pid] = cb;
    mHandlers[pid] = hdl;
}

void TsDemuxer::addPesPid(int pid, PesCallBackFnc cb, void* hdl)
{
    mPesCallbackMap[pid] = cb;
    mHandlers[pid] = hdl;
}

void TsDemuxer::addTsPid(int pid, TsCallBackFnc cb, void* hdl)
{
    mTsCallbackMap[pid] = cb;
    mHandlers[pid] = hdl;
}
