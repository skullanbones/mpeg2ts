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

void TsDemuxer::demux(const uint8_t* packet)
{
    TsPacketInfo tsPacketInfo;
    mParser.parseTsPacketInfo(packet, tsPacketInfo);
    if (mPsiCallbackMap.find(tsPacketInfo.pid) != mPsiCallbackMap.end())
    {
        // TODO Filter PID from PSI, TS, PES etc...
        // Check what table
        PsiTable psi;
        mParser.parsePsiTable(packet, tsPacketInfo, psi);

        if (tsPacketInfo.pid == TS_PACKET_PID_PAT) // TODO also use table_id?
        {
            PatTable table;
            table = mParser.parsePatPacket(packet, tsPacketInfo);
            mPsiCallbackMap[tsPacketInfo.pid](table);
        }
        else if (psi.table_id == PSI_TABLE_ID_PMT)
        {
            PmtTable table;
            table = mParser.parsePmtPacket(packet, tsPacketInfo);
            mPsiCallbackMap[tsPacketInfo.pid](table);
        }

        // TODO: gather whole table and send it then
    }
}

void TsDemuxer::addPsiPid(int pid, PsiCallBackFnc cb)
{
    mPsiCallbackMap[pid] = cb;
}
