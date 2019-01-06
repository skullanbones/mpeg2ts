#pragma once


#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "public/mpeg2ts.h"


// Forward decl
namespace mpeg2
{
class Mpeg2VideoEsParser;
}
namespace h264
{
class H264EsParser;
}

/*
 * High level API on mpeg2ts library
 * Utilities functionality to simplify usage of mpeg2ts library
 */

namespace tsutil
{

class IpAddress
{
public:
    explicit IpAddress(const std::string& ip)
        : mIpAddress(ip)
    {
    }
    std::string GetIpAddress() const
    {
        return mIpAddress;
    }

private:
    std::string mIpAddress;
};

class Port
{
public:
    explicit Port(const std::string& port)
        : mPort(port)
    {
    }
    std::string GetPort() const
    {
        return mPort;
    }

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

enum class MediaType
{
    Audio,
    Video,
    Private,
    Unknown
};

enum class VideoCodecType
{
    MPEG2,
    H264
};

struct VideoMediaInfo
{
    MediaType mediaType{ MediaType::Unknown };
    int PID{ -1 };
    VideoCodecType codec;
    int width;
    int height;
    std::string frameRate;
};

class TsUtilities
{
public:
    MPEG2TS_API explicit TsUtilities();

    MPEG2TS_API ~TsUtilities();

    TsUtilities(const TsUtilities&) = delete;
    const TsUtilities& operator=(const TsUtilities&) = delete;
    TsUtilities(const TsUtilities&&) = delete;
    const TsUtilities& operator=(const TsUtilities&&) = delete;

    //* Parsing functionality *//

    //! \brief Parses a file of Transport Stream packets.
    //! \param file The binary file with a transport stream
    //! \return True if parse was successful, false in all other cases
    MPEG2TS_API bool parseTransportFile(const std::string& file);

    //! \brief Parses data over a network transmitting UDP packets containing Transport Stream
    //! packets. \return True if parse was successful, false in all other cases \note NOT
    //! IMPLEMENTED!!!!!!!!!!!!! \note this API will stream data to callbacks... TODO TBD
    MPEG2TS_API bool parseTransportUdpStream(const IpAddress& ip, const Port& p);

    //! \brief Parses a raw data buffer of Transport Stream packets.
    //! \param data Raw pointer to data buffer to parse. Must be binary.
    //! \param size The size of the buffer to parse.
    //! \return True if parse was successful, false in all other cases
    MPEG2TS_API bool parseTransportStreamData(const uint8_t* data, std::size_t size);

    //* callbacks *//
    static void PATCallback(const mpeg2ts::ByteVector& rawTable, mpeg2ts::PsiTable* table, int pid, void* hdl);
    static void PMTCallback(const mpeg2ts::ByteVector& rawTable, mpeg2ts::PsiTable* table, int pid, void* hdl);
    static void PESCallback(const mpeg2ts::ByteVector& rawPes, const mpeg2ts::PesPacket& pes, int pid, void* hdl);

    //* PAT *//
    //! \brief Returns the PAT table found in stream
    //! \return The PAT table
    MPEG2TS_API mpeg2ts::PatTable getPatTable() const;

    //* PMT *//
    //! \brief Returns a vector with all PMT PIDs found in stream
    //! \note NOT Orthogonal API, can be extracted from PAT... getPatTable API TODO REMOVE??
    //! \return Vector containing PMT PIDs
    MPEG2TS_API std::vector<uint16_t> getPmtPids() const;

    //! \brief Returns a map with all PMT tables found in stream
    //! \return Map containing PMTs ordered by their respective PID as keys
    MPEG2TS_API std::map<int, mpeg2ts::PmtTable> getPmtTables() const;

    //* ES / PES *//
    //! \brief Returns a vector with all Elementary Stream PIDs found in stream
    //! \note NOT orthogonal API, can be extracted from PMTTables.. TODO remove?
    //! \return Vector with ES PIDs
    MPEG2TS_API std::vector<uint16_t> getEsPids() const;

    //! \brief Returns a map with all PES packets found in stream
    //! \return Map containing PES packets ordered by their respective PID as keys
    MPEG2TS_API std::map<int, std::vector<mpeg2ts::PesPacket>> getPesPackets() const;

    MPEG2TS_API mpeg2ts::PidStatisticsMap getPidStatistics() const;

    MPEG2TS_API VideoMediaInfo getVideoMediaInfo() const;

    MPEG2TS_API std::string toString(MediaType e) const;

    MPEG2TS_API std::string toString(VideoCodecType e) const;

private:
    void initLogging() const;
    void initParse();
    void registerPmtCallback();
    void registerPesCallback();

    // Default constants
    static constexpr const LogLevel DEFAULT_LOG_LEVEL{ LogLevel::DEBUG };
    static const std::string LOGFILE_NAME; // = "mpeg2ts_log.csv";
    static constexpr const int LOGFILE_MAXSIZE{ 100 * 1024 };
    static constexpr const int LOGFILE_MAXNUMBEROF{ 10 };

    // members
    mpeg2ts::TsDemuxer mDemuxer;
    mpeg2ts::PatTable mPrevPat;
    std::vector<uint16_t> mPmtPids;
    std::map<int, mpeg2ts::PmtTable> mPmts;
    std::vector<uint16_t> mEsPids;
    bool mAddedPmts;
    VideoMediaInfo mVideoMediaInfo;

    std::map<int, std::vector<mpeg2ts::PesPacket>> mPesPackets;
    std::unique_ptr<mpeg2::Mpeg2VideoEsParser> m_Mpeg2Parser;
    std::unique_ptr<h264::H264EsParser> m_H264Parser;
};

inline VideoMediaInfo TsUtilities::getVideoMediaInfo() const
{
    return mVideoMediaInfo;
}

inline std::string TsUtilities::toString(MediaType e) const
{
    const std::map<MediaType, std::string> MyEnumStrings{ { MediaType::Audio, "Audio" },
                                                          { MediaType::Video, "Video" },
                                                          { MediaType::Private, "Private" },
                                                          { MediaType::Unknown, "Unknown" } };
    auto it = MyEnumStrings.find(e);
    return it == MyEnumStrings.end() ? "Out of range" : it->second;
}

inline std::string TsUtilities::toString(VideoCodecType e) const
{
    const std::map<VideoCodecType, std::string> MyEnumStrings{ { VideoCodecType::MPEG2, "MPEG2" },
                                                               { VideoCodecType::H264, "H264" } };
    auto it = MyEnumStrings.find(e);
    return it == MyEnumStrings.end() ? "Out of range" : it->second;
}

} // namespace tsutil
