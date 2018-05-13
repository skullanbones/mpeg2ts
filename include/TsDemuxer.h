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

/// @brief Demux ts packets into PSI and PES (plus TS pass through)
class TsDemuxer
{
public:
    typedef std::function<void(PsiTable* table, void* hdl)> PsiCallBackFnc;
    typedef std::function<void(const PesPacket& table, uint16_t pid, void* hdl)> PesCallBackFnc;
    typedef std::function<void(const uint8_t* packet, TsPacketInfo tsPacketInfo, void* hdl)> TsCallBackFnc;

    TsDemuxer() = default;

    /*!
     * Demuxes a transport stream packed based on its Packet ID.
     * @param tsPacket The TS packet to demux.
     */
    void demux(const uint8_t* tsPacket);

    /*!
     * Outputs to callback function when found a PSI table with PID.
     * @param pid The PID to filter PSI tables.
     * @param cb Returns a complete PSI table to this callback function.
     * @param hdl Custom handler
     */
    void addPsiPid(int pid, PsiCallBackFnc cb, void* hdl);

    /*!
     * Returns a complete PES packet with PID.
     * @param pid The PID to filter PES packets.
     * @param cb  Callback when found a complete PES packet.
     * @param hdl Custom handler
     */
    void addPesPid(int pid, PesCallBackFnc cb, void* hdl);

    /*!
     * Returns a complete TS packet filtered on PID.
     * @param pid The PID to filter TS packets.
     * @param cb Callback when found a TS packet.
     * @param hdl Custom handler
     */
    void addTsPid(int pid, TsCallBackFnc cb, void* hdl);

    /*!
    * Returns statistics on parsed transport stream packets.
    * @return TsStatistics containing collected statistics for all demuxed packets.
    */
    const TsStatistics getTsStatistics() const
    {
        return static_cast<TsStatistics>(mParser);
    }

protected:
    std::map<int, PsiCallBackFnc> mPsiCallbackMap;
    std::map<int, PesCallBackFnc> mPesCallbackMap;
    std::map<int, TsCallBackFnc> mTsCallbackMap;
    std::map<int, void*> mHandlers;

private:
    TsParser mParser;
};
