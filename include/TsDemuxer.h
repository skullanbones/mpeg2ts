/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */

#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <stdio.h>

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

    TsDemuxer();

    void demux(const uint8_t* packet);

    void addPsiPid(int pid, PsiCallBackFnc cb);
    void addPesPid(int pid, PesCallBackFnc cb);
    void addTsPid(int pid, TsCallBackFnc cb);

protected:
    std::map<int, PsiCallBackFnc> mPsiCallbackMap; // TODO: make cb generic
    std::map<int, PesCallBackFnc> mPesCallbackMap;


private:
    TsParser mParser;
};
