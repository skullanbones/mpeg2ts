/**
* Strictly Confidential - Do not duplicate or distribute without written
* permission from authors
*/

#pragma once
#ifdef _WIN32

#ifdef TSLIB_DLL_EXPORTS  
#define TSLIB_API __declspec(dllexport)   
#else  
#define TSLIB_API __declspec(dllimport)   
#endif

#elif __linux__
#define TSLIB_API
#endif



#include <functional>
#include <map>

/// project files
#include "PsiTables.h"
#include "PesPacket.h"
#include "TsPacketInfo.h"
//#include "TsStatistics.h"

// forward declarations
class TsParser;

namespace tslib {

typedef std::function<void(PsiTable* table, uint16_t pid, void* hdl)> PsiCallBackFnc;
typedef std::function<void(const PesPacket& table, uint16_t pid, void* hdl)> PesCallBackFnc;
typedef std::function<void(const uint8_t* packet, TsPacketInfo tsPacketInfo, void* hdl)> TsCallBackFnc;

/// @brief Demux ts packets into PSI and PES (plus TS pass through)
class TsDemuxer
{
public:
    TSLIB_API TsDemuxer();

    TSLIB_API ~TsDemuxer();

    /*!
    * Demuxes a transport stream packed based on its Packet ID.
    * @param tsPacket The TS packet to demux.
    */
    TSLIB_API void demux(const uint8_t* tsPacket);

    /*!
    * Outputs to callback function when found a PSI table with PID.
    * @param pid The PID to filter PSI tables.
    * @param cb Returns a complete PSI table to this callback function.
    * @param hdl Custom handler
    */
    TSLIB_API void addPsiPid(int pid, PsiCallBackFnc cb, void* hdl);

    /*!
    * Returns a complete PES packet with PID.
    * @param pid The PID to filter PES packets.
    * @param cb  Callback when found a complete PES packet.
    * @param hdl Custom handler
    */
    TSLIB_API void addPesPid(int pid, PesCallBackFnc cb, void* hdl);

    /*!
    * Returns a complete TS packet filtered on PID.
    * @param pid The PID to filter TS packets.
    * @param cb Callback when found a TS packet.
    * @param hdl Custom handler
    */
    TSLIB_API void addTsPid(int pid, TsCallBackFnc cb, void* hdl);

    /*!
    * Returns statistics on parsed transport stream packets.
    * @return TsStatistics containing collected statistics for all demuxed packets.
    */
    /*TSLIB_API TsStatistics getTsStatistics() const
    {
    	return static_cast<TsStatistics>(*mParser.get());
    }*/

protected:
    std::map<int, PsiCallBackFnc> mPsiCallbackMap;
    std::map<int, PesCallBackFnc> mPesCallbackMap;
    std::map<int, TsCallBackFnc> mTsCallbackMap;
    std::map<int, void*> mHandlers;

private:
    // Make this object be non copyable because it holds a pointer
    TsDemuxer(const TsDemuxer &);
    const TsDemuxer &operator =(const TsDemuxer &);

    TsParser* mParser;
};

}