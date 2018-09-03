#pragma once


#include <cstdint>
#include <cstddef>
#include <string>

#include "public/mpeg2ts.h"

/*
 * High level API on mpeg2ts library
 * Utilities functionality to simplify usage of mpeg2ts library
 */

namespace tsutil
{
using namespace mpeg2ts;

class TsUtilities
{
public:
    MPEG2TS_API explicit  TsUtilities();

    MPEG2TS_API ~TsUtilities() = default;

    MPEG2TS_API bool parseTransportFile(const std::string& file);

    // TODO MPEG2TS_API parseTransportUdpStream();

    MPEG2TS_API bool parseTransportStreamData(const uint8_t* data, std::size_t size);

    // callbacks
    static void PATCallback(PsiTable* table, uint16_t pid, void* hdl);
    static void PMTCallback(PsiTable* table, uint16_t pid, void* hdl);

    // PAT
    MPEG2TS_API PatTable getPatTable() const;

    MPEG2TS_API std::vector<uint16_t> getPmtPids() const;

    // PMT
    MPEG2TS_API std::map<uint16_t, PmtTable> getPmtTables() const;

private:
    void initLogging();
    void initParse();
    void registerPmtCallback();

    mpeg2ts::TsDemuxer mDemuxer;
    PatTable mPrevPat;
    std::vector<uint16_t> mPmtPids;
    std::map<uint16_t, PmtTable> mPmts;
    std::vector<uint16_t> mEsPids;
    bool mAddedPmts;
};

}