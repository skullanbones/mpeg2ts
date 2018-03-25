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
    typedef std::function<void(const TsHeader& hdr)> TsCallBackFnc;

    TsDemuxer() = default;

    void demux(const uint8_t* tsPacket);

    void addPsiPid(int pid, PsiCallBackFnc cb);
    void addPesPid(int pid, PesCallBackFnc cb);
    void addTsPid(int pid, TsCallBackFnc cb);

protected:
    std::map<int, PsiCallBackFnc> mPsiCallbackMap; // TODO: make cb generic
    std::map<int, PesCallBackFnc> mPesCallbackMap;


private:
    TsParser mParser;
};
