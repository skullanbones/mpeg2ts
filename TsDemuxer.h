/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */

#pragma once
#include "TsParser.h"
#include <iostream>
#include <stdio.h>
#include <functional>
#include <map>

/// @brief Base class for all tables
// TODO: move to own file
class PsiTable
{
public:
    uint8_t id;
};

/// @brief Parsed PES
// TODO: move to own file
class PesPacket
{
protected:
};

/// @brief Demux ts packets into PSI and PES (plus passthrough)
class TsDemuxer
{
public:
    typedef std::function<void(const PsiTable& table)> PSICallBackFnc;
    //TODO: add 2 more

    TsDemuxer();

    void demux(const unsigned char* packet, const TsPacketInfo& tsPacketInfo);
    void addPid(int pid, PSICallBackFnc cb);
protected:
    std::map<int, PSICallBackFnc> mCallbackMap;//TODO: make cb generic
};
