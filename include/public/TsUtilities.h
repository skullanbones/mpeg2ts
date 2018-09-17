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

class IpAddress
{
public:
    explicit IpAddress(const std::string& ip) : mIpAddress(ip) {}
    std::string GetIpAddress() const { return mIpAddress; }
private:
    std::string mIpAddress;
};

class Port
{
public:
    explicit Port(const std::string& port) : mPort(port) {}
    std::string GetPort() const { return mPort; }
private:
    std::string mPort;
};

enum class LogLevel
{
    VERBOSE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL,
    NONE
};

class TsUtilities
{
public:
    MPEG2TS_API explicit TsUtilities();

    MPEG2TS_API ~TsUtilities() = default;

    TsUtilities(const TsUtilities&) = delete;
    const TsUtilities& operator=(const TsUtilities&) = delete;
    TsUtilities(const TsUtilities&&) = delete;
    const TsUtilities& operator=(const TsUtilities&&) = delete;

    //* Parsing functionality *//

    //! \brief Parses a file of Transport Stream packets.
    //! \param file The binary file with a transport stream
    //! \return True if parse was successful, false in all other cases
    MPEG2TS_API bool parseTransportFile(const std::string& file);

    //! \brief Parses data over a network transmitting UDP packets containing Transport Stream packets.
    //! \return True if parse was successful, false in all other cases
    //! \note NOT IMPLEMENTED!!!!!!!!!!!!!
    //! \note this API will stream data to callbacks... TODO TBD
    MPEG2TS_API bool parseTransportUdpStream(const IpAddress &ip, const Port &p);

    //! \brief Parses a raw data buffer of Transport Stream packets.
    //! \param data Raw pointer to data buffer to parse. Must be binary.
    //! \param size The size of the buffer to parse.
    //! \return True if parse was successful, false in all other cases
    MPEG2TS_API bool parseTransportStreamData(const uint8_t* data, std::size_t size);

    //* callbacks *//
    static void PATCallback(PsiTable* table, uint16_t pid, void* hdl);
    static void PMTCallback(PsiTable* table, uint16_t pid, void* hdl);
    static void PESCallback(const PesPacket& pes, uint16_t pid, void* hdl);

    //* PAT *//
    //! \brief Returns the PAT table found in stream
    //! \return The PAT table
    MPEG2TS_API PatTable getPatTable() const;

    //* PMT *//
    //! \brief Returns a vector with all PMT PIDs found in stream
    //! \note NOT Orthogonal API, can be extracted from PAT... getPatTable API TODO REMOVE??
    //! \return Vector containing PMT PIDs
    MPEG2TS_API std::vector<uint16_t> getPmtPids() const;

    //! \brief Returns a map with all PMT tables found in stream
    //! \return Map containing PMTs ordered by their respective PID as keys
    MPEG2TS_API std::map<uint16_t, PmtTable> getPmtTables() const;

    //* ES / PES *//
    //! \brief Returns a vector with all Elementary Stream PIDs found in stream
    //! \note NOT orthogonal API, can be extracted from PMTTables.. TODO remove?
    //! \return Vector with ES PIDs
    MPEG2TS_API std::vector<uint16_t> getEsPids() const;

    //! \brief Returns a map with all PES packets found in stream
    //! \return Map containing PES packets ordered by their respective PID as keys
    MPEG2TS_API std::map<uint16_t, std::vector<PesPacket>> getPesPackets() const;

private:
    void initLogging() const;
    void initParse();
    void registerPmtCallback();
    void registerPesCallback();

    // Default constants
    static const LogLevel DEFAULT_LOG_LEVEL; // = LogLevel::DEBUG;
    static const std::string LOGFILE_NAME; // = "mpeg2ts_log.csv";
    static int LOGFILE_MAXSIZE; // = 100 * 1024;
    static int LOGFILE_MAXNUMBEROF; // = 10;

    // members
    TsDemuxer mDemuxer;
    PatTable mPrevPat;
    std::vector<uint16_t> mPmtPids;
    std::map<uint16_t, PmtTable> mPmts;
    std::vector<uint16_t> mEsPids;
    bool mAddedPmts;
    std::map<uint16_t, std::vector<PesPacket>> mPesPackets;
};

} // namespace tsutil
