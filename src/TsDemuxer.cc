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
#include <stdint.h>

TsDemuxer::TsDemuxer()
{
}

void TsDemuxer::demux(const uint8_t* tsPacket)
{
    TsPacketInfo tsPacketInfo;
    mParser.parseTsPacketInfo(tsPacket, tsPacketInfo);
    if (mPsiCallbackMap.find(tsPacketInfo.pid) != mPsiCallbackMap.end())
    {
        // TODO Filter PID from PSI, TS, PES etc...
        // Check what table
        auto table = mParser.collectTable(tsPacket, tsPacketInfo);

        if (table.empty())
        {
            return;
        }
        
        PsiTable tableInfo = mParser.parsePsiTable(table);
        if (tableInfo.table_id == PSI_TABLE_ID_PAT)
        {
            PatTable pat = mParser.parsePatPacket(table);
            mPsiCallbackMap[tsPacketInfo.pid](pat);
        }
        else if (tableInfo.table_id == PSI_TABLE_ID_PMT)
        {
            PmtTable pmt = mParser.parsePmtPacket(table);
            mPsiCallbackMap[tsPacketInfo.pid](pmt);
        }
    }
}

void TsDemuxer::addPsiPid(int pid, PsiCallBackFnc cb)
{
    mPsiCallbackMap[pid] = cb;
}
