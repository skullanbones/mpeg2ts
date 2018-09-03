#pragma once

#ifdef _WIN32

#ifdef TSLIB_DLL_EXPORTS
#define MPEG2TS_API __declspec(dllexport)
#else
#define MPEG2TS_API __declspec(dllimport)
#endif

#elif __linux__
#define MPEG2TS_API
#endif

#include <cstdint>
#include <cstddef>

#include "public/mpeg2ts.h"

/*
 * High level API on mpeg2ts library
 * Utilities functionality to simplify usage of mpeg2ts library
 */

namespace tsutil
{

class TsUtilities
{
public:
    MPEG2TS_API explicit  TsUtilities() = default;

    MPEG2TS_API ~TsUtilities() = default;

    // TODO parseTransportFile(std::url ...);

    // TODO parseTransportUdpStream();

    MPEG2TS_API bool parseTransportStreamData(const uint8_t* data, std::size_t size);

    static void PATCallback(PsiTable* table, uint16_t pid, void* hdl);

    MPEG2TS_API PatTable getPatTable() const;

    MPEG2TS_API std::vector<uint16_t> getPmtPids() const;

private:
    mpeg2ts::TsDemuxer mDemuxer;
    PatTable mPrevPat;
    std::vector<uint16_t> mPmtPids;
};

}