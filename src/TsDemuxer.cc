/**
 * @class TsParser
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
    if (mCallbackMap.find(tsPacketInfo.pid) != mCallbackMap.end())
    {
        // TODO Filter PID from PSI, TS, PES etc...
        PsiTable table;

        if (tsPacketInfo.pid == TS_PACKET_PID_PAT)
        {
            table = mParser.parsePatPacket(packet, tsPacketInfo);
        }

        // TODO: gather whole table and send it then
        mCallbackMap[tsPacketInfo.pid](table);
    }
}

void TsDemuxer::addPid(int pid, PsiCallBackFnc cb)
{
    mCallbackMap[pid] = cb;
}
