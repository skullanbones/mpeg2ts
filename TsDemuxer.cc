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

void TsDemuxer::demux(const unsigned char* packet, const TsPacketInfo& tsPacketInfo)
{
    if (mCallbackMap.find(tsPacketInfo.pid) != mCallbackMap.end())
    {
        PsiTable table;
        // TODO: gather whole table and send it then
        mCallbackMap[tsPacketInfo.pid](table);
    }
}

void TsDemuxer::addPid(int pid, PsiCallBackFnc cb)
{
    mCallbackMap[pid] = cb;
}
