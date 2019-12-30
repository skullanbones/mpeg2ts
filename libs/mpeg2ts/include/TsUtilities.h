/*****************************************************************
*
*    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
*
*    mpeg2ts - mpeg2ts lib
*
*    This file is part of mpeg2ts (Mpeg2 Transport Stream Library).
*
*    Unless you have obtained mpeg2ts under a different license,
*    this version of mpeg2ts is mpeg2ts|GPL.
*    Mpeg2ts|GPL is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License as
*    published by the Free Software Foundation; either version 2,
*    or (at your option) any later version.
*
*    Mpeg2ts|GPL is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with mpeg2ts|GPL; see the file COPYING.  If not, write to the
*    Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
*    02111-1307, USA.
*
********************************************************************/
#ifndef _TS_UTILITIES_H
#define _TS_UTILITIES_H

#include <cstdint>              // for uint16_t, uint8_t                                                                                                          
#include <cstddef>              // for size_t            
#include <map>                  // for map, _Rb_tree_const_iterator                                                                                             
#include <memory>               // for unique_ptr                                                                                                                    
#include <string>               // for string, basic_string
#include <utility>              // for pair                                                                     
#include <vector>               // for vector                                                                                                                 

#include "mpeg2ts.h"            // for MPEG2TS_API, ByteVector, PesPacket (p...
#include "mpeg2ts_export.h"     // for exporting as DLL

/*!
 * @brief High level API on mpeg2ts library
 * Utilities functionality to simplify usage of mpeg2ts library
 */
namespace tsutil
{

/*!
 * @brief IP address to use for input of TS stream
 */
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

/*!
 * @brief IP port to use for input of TS stream
 */
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

/*!
 * @brief Set log-level verbosity
 */
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

/*!
 * @brief Type of media
 */
enum class MediaType
{
    Audio,
    Video,
    Private,
    Unknown
};

/*!
 * @brief Type of codec
 * @todo Implement more codecs (H.265, AV1 etc)
 */
enum class VideoCodecType
{
    MPEG2,
    H264
};

/*!
 * @brief Video meta data found from codecs currently only H.264 and MPEG2 Video
 */
struct VideoMediaInfo
{
    // Common to codecs
    MediaType mediaType{ MediaType::Unknown };
    int PID{ -1 };
    VideoCodecType codec;
    int width;
    int height;
    std::string frameRate;

    // Mpeg2
    uint64_t picType;
    std::string aspect;

    // H264
    int lumaBits;
    int chromaBits;
    int numRefPics;
};

typedef std::function<void(const std::vector<uint8_t>& frame, int streamType)> VideoCallBackFnc;

/*!
 * @brief High level API for TsUtilities
 */
class TsUtilities
{
public:
    MPEG2TS_EXPORT explicit TsUtilities();

    MPEG2TS_EXPORT ~TsUtilities();

    // Disable copy ctor
    TsUtilities(const TsUtilities&) = delete;
    const TsUtilities& operator=(const TsUtilities&) = delete;
    // Disable move ctor
    TsUtilities(const TsUtilities&&) = delete;
    const TsUtilities& operator=(const TsUtilities&&) = delete;

    //* Parsing functionality *//

    /*!
     * @brief Parses a file of Transport Stream packets.
     * @param file The binary file with a transport stream
     * @return True if parse was successful, false in all other cases
     */
    MPEG2TS_EXPORT bool parseTransportFile(const std::string& file);

    /*!
     * @brief Parses data over a network transmitting UDP packets containing Transport Stream packets. 
     * @return True if parse was successful, false in all other cases 
     * @note NOT IMPLEMENTED! TODO: this API will stream data to callbacks...
     * @todo Not implemented
     */
    MPEG2TS_EXPORT bool parseTransportUdpStream(const IpAddress& ip, const Port& p);

    /*!
     * @brief Parses a raw data buffer of Transport Stream packets.
     * @param data Raw pointer to data buffer to parse. Must be binary.
     * @param size The size of the buffer to parse.
     * @return True if parse was successful, false in all other cases
     */
    MPEG2TS_EXPORT bool parseTransportStreamData(const uint8_t* data, std::size_t size);

    /*!
     * @brief Returns the PAT table found in stream
     * @return The PAT table
     */
    MPEG2TS_EXPORT mpeg2ts::PatTable getPatTable() const;

    /*!
     * @brief Returns a vector with all PMT PIDs found in stream
     * @note NOT Orthogonal API, can be extracted from PAT... getPatTable 
     * @todo API REMOVE??
     * @return Vector containing PMT PIDs
     */
    MPEG2TS_EXPORT std::vector<uint16_t> getPmtPids() const;

    /*!
     * @brief Returns a map with all PMT tables found in stream
     * @return Map containing PMTs ordered by their respective PID as keys
     */
    MPEG2TS_EXPORT std::map<int, mpeg2ts::PmtTable> getPmtTables() const;

    /*!
     * @brief Returns a vector with all Elementary Stream PIDs found in stream
     * @note NOT orthogonal API, can be extracted from PMTTables..
     * @todo remove?
     * @return Vector with ES PIDs
     */
    MPEG2TS_EXPORT std::vector<uint16_t> getEsPids() const;

    /*!
     * @brief Returns a map with all PES packets found in stream
     * @return Map containing PES packets ordered by their respective PID as keys
     */
    MPEG2TS_EXPORT std::map<int, std::vector<mpeg2ts::PesPacket>> getPesPackets() const;

    /*!
     * @brief Returns statistics per PID (time-stamps, continuity counters)
     * @return Map containing statistics ordered by their respective PID as keys
     */
    MPEG2TS_EXPORT mpeg2ts::PidStatisticsMap getPidStatistics() const;

    /*!
     * @brief Returns video codec meta data
     * @return Codec (H.264/MPEG2) video codec meta data
     */
    MPEG2TS_EXPORT VideoMediaInfo getVideoMediaInfo() const;

    /*!
     * @brief Convert MediaType to string
     * @return MediaType string
     */
    MPEG2TS_EXPORT std::string toString(MediaType e) const;

    /*!
     * @brief Convert VideoCodecType to string
     * @return VideoCodecType string
     */
    MPEG2TS_EXPORT std::string toString(VideoCodecType e) const;

    /*!
     * @brief Callback for each video frame for codec parsing functionality (Use h264codec/mpeg2codec libs)
     * @param Callback function to be called
     */
    MPEG2TS_EXPORT void addVideoCallback(VideoCallBackFnc cb);

private:
    //* callbacks *//
    static void PATCallback(const mpeg2ts::ByteVector& rawTable, mpeg2ts::PsiTable* table, int pid, void* hdl);
    static void PMTCallback(const mpeg2ts::ByteVector& rawTable, mpeg2ts::PsiTable* table, int pid, void* hdl);
    static void PESCallback(const mpeg2ts::ByteVector& rawPes, const mpeg2ts::PesPacket& pes, int pid, void* hdl);

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

    VideoCallBackFnc mVideoCallback;
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

#endif /* _TS_UTILITIES_H */
