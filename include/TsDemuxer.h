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


/// @brief Demux ts packets into PSI and PES (plus passthrough)
class TsDemuxer
{
public:
    typedef std::function<void(const PsiTable& table)> PsiCallBackFnc;
    // TODO: add 2 more

    TsDemuxer();

    void demux(const uint8_t* packet);
    void addPid(int pid, PsiCallBackFnc cb);

protected:
    std::map<int, PsiCallBackFnc> mCallbackMap; // TODO: make cb generic


private:
    TsParser mParser;
};
