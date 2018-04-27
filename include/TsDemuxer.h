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
#include "TsStandards.h"

/// @brief Demux ts packets into PSI and PES (plus TS pass through)
class TsDemuxer
{
public:
    typedef std::function<void(PsiTable* table)> PsiCallBackFnc;
    typedef std::function<void(const PesPacket& table, uint16_t pid)> PesCallBackFnc;
    typedef std::function<void(const uint8_t* packet, TsPacketInfo tsPacketInfo)> TsCallBackFnc;

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
     */
    void addPsiPid(int pid, PsiCallBackFnc cb);

    /*!
     * Returns a complete PES packet with PID.
     * @param pid The PID to filter PES packets.
     * @param cb  Callback when found a complete PES packet.
     */
    void addPesPid(int pid, PesCallBackFnc cb);

    /*!
     * Returns a complete TS packet filtered on PID.
     * @param pid The PID to filter TS packets.
     * @param cb Callback when found a TS packet.
     */
    void addTsPid(int pid, TsCallBackFnc cb);

    /*!
    * Returns statistics on parsed transport stream packets.
    * @return TsStatistics containing collected statistics for all demuxed packets.
    */
    const TsStatistics getTsStatistics() const
    {
        return static_cast<TsStatistics>(mParser);
    }

protected:
    std::map<int, PsiCallBackFnc> mPsiCallbackMap; // TODO: make cb generic
    std::map<int, PesCallBackFnc> mPesCallbackMap;
    std::map<int, TsCallBackFnc> mTsCallbackMap;

private:
    TsParser mParser;
};
