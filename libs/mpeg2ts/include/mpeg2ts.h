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
#ifndef _MPEG2TS_H
#define _MPEG2TS_H

#include "mpeg2ts_export.h"  // For __declspec(dllexport/dllimport)

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <vector>

// project files
#include "Ts_IEC13818-1.h" // For TsHeader  Program


namespace mpeg2ts
{
// Forward declarations
class TsParser;
struct PidStatistic;

// Common types
typedef std::vector<uint8_t> ByteVector;
typedef std::shared_ptr<ByteVector> ByteVectorPtr;
typedef std::map<int, PidStatistic> PidStatisticsMap;


/*!
 * @brief PES-Packet prototype containing buffer
 * @ref ISO/IEC 13818-1 (Table 2-21 – PES packet)
 */
struct PesPacket
{
    uint32_t packet_start_code_prefix;
    uint8_t stream_id;
    uint16_t PES_packet_length;

    // Extended packet
    bool PES_scrambling_control;
    bool PES_priority;
    bool data_alignment_indicator;
    bool copyright;
    bool original_or_copy;
    uint8_t PTS_DTS_flags;
    bool ESCR_flag;
    bool ES_rate_flag;
    bool DSM_trick_mode_flag;
    bool additional_copy_info_flag;
    bool PES_CRC_flag;
    bool PES_extension_flag;

    uint8_t PES_header_data_length;
    uint16_t elementary_data_offset; // where ES data begins

    int64_t pts;
    int64_t dts;

    ByteVector mPesBuffer;

    MPEG2TS_EXPORT friend std::ostream& operator<<(std::ostream& ss, const PesPacket& rhs);
};


/*!
 * @brief Base class for all PSI tables
 */
class PsiTable
{
public:
    PsiTable()
    {
        table_id = 0;
        section_syntax_indicator = false;
        section_length = 0;
        transport_stream_id = 0;
        version_number = 0;
        current_next_indicator = false;
        section_number = 0;
        last_section_number = 0;
        CRC_32 = 0;
    }

    virtual ~PsiTable() = default;

    uint8_t table_id;
    bool section_syntax_indicator;
    uint16_t section_length;
    uint16_t transport_stream_id; // TODO program_number for PMT? Use Union?
    uint8_t version_number;
    bool current_next_indicator;
    uint8_t section_number;
    uint8_t last_section_number;
    uint32_t CRC_32;

    MPEG2TS_EXPORT friend std::ostream& operator<<(std::ostream& ss, const PsiTable& rhs);

    /// @brief Comparison operator for comparing 2 PsiTables
    MPEG2TS_EXPORT bool operator==(const PsiTable& rhs) const;

    MPEG2TS_EXPORT bool operator!=(const PsiTable& rhs) const;
};

/*!
 * @brief PAT Table – Program association section
 * @ref ISO/IEC 13818-1 (Table 2-30)
 */
class PatTable : public PsiTable
{
public:
    std::vector<Program> programs;

    MPEG2TS_EXPORT friend std::ostream& operator<<(std::ostream& ss, const PatTable& rhs);

    /// @brief Comparison operator for comparing 2 PatTables
    MPEG2TS_EXPORT bool operator==(const PatTable& rhs) const;

    MPEG2TS_EXPORT bool operator!=(const PatTable& rhs) const;
};

/*!
 * @brief Part of PMT Table
 * @ref ISO/IEC 13818-1 (Table 2-33)
 */
struct StreamTypeHeader
{
    StreamType stream_type;
    uint16_t elementary_PID;
    uint16_t ES_info_length;

    friend std::ostream& operator<<(std::ostream& ss, const StreamTypeHeader& rhs);

    bool operator==(const StreamTypeHeader& rhs) const;

    bool operator!=(const StreamTypeHeader& rhs) const;
};

/*!
 * @brief PMT Table – Transport stream program map section 
 * @ref ISO/IEC 13818-1 (Table 2-33)
 */
class PmtTable : public PsiTable
{
public:
    PmtTable()
        : PCR_PID(0)
        , program_info_length(0)
    {
    }

    uint16_t PCR_PID;
    uint16_t program_info_length;
    std::vector<Descriptor> descriptors;
    std::vector<StreamTypeHeader> streams;


    MPEG2TS_EXPORT friend std::ostream& operator<<(std::ostream& ss, const PmtTable& rhs);

    /// @brief Comparison operator for comparing 2 PmtTables
    MPEG2TS_EXPORT bool operator==(const PmtTable& rhs) const;

    MPEG2TS_EXPORT bool operator!=(const PmtTable& rhs) const;
};

/*!
 * @brief Conditional access table
 * @ref Table 2-32 – Conditional access section
 * @todo Not implemented
 */
class CatTable : public PsiTable
{
public:
    std::vector<CatDescriptor> descriptors;
};

/*!
 * @brief Parsed meta data for 1 TS packet
 */
class TsPacketInfo
{
public:
    TsPacketInfo()
        : pid{ 0x1fff }
        , errorIndicator{ false }
        , isPayloadStart{ false }
        , hasAdaptationField{ false }
        , hasPayload{ false }
        , hasPrivateData{ false }
        , hasRandomAccess{ false }
        , isScrambled{ false }
        , isDiscontinuity{ false }
        , continuityCounter{ 0 }
        , pcr{ -1 }
        , opcr{ -1 }
        , pts{ -1 }
        , dts{ -1 }
        , privateDataSize{ 0 }
        , privateDataOffset{ 0 }
        , payloadSize{ 0 }
        , payloadStartOffset{ 0 }
        , isError{ false }
    {
    }

    int pid;      // This Packet Identifier.
    TsHeader hdr; // This packet Ts Header

    bool errorIndicator;     // If indication of at least 1 uncorrectable bit in ts-packet
    bool isPayloadStart;     // If this packet is the first in a PES-packet.
    bool hasAdaptationField; // If this packet contains adaptation field data.
    bool hasPayload;         // If this packet contains payload.
    bool hasPrivateData;     // If this packet contains private data.
    bool hasRandomAccess;    // If this packet contain aid random access.
    bool isScrambled;        // If this packet is scrambled
    bool isDiscontinuity;    // If this packet has discontinuity flag set

    uint8_t continuityCounter; // 4-bit continuity counter

    // Adaptation field data:
    int64_t pcr; // PCR clock...
    int64_t opcr;

    // For elementary-streams
    int64_t pts;
    int64_t dts;

    // Private data:
    uint32_t privateDataSize;
    uint32_t privateDataOffset;

    // Payload data:
    std::size_t payloadSize;    // The size of the payload
    uint8_t payloadStartOffset; // Offset from and sync byte to start of payload.

    bool isError; // If a parser error or TS not following standards.


    MPEG2TS_EXPORT friend std::ostream& operator<<(std::ostream& ss, const TsPacketInfo& rhs);
};

/*!
 * @brief Statistics per PID with time-stamp variations (DTS, PTS, PCR) and 
 * continuity counters.
 */
struct PidStatistic
{
    static constexpr const uint8_t INVALID_CC{ 16 };
    PidStatistic()
        : lastCC{ INVALID_CC }
        , numberOfCCErrors{ 0 }
        , numberOfTsDiscontinuities{ 0 }
        , lastPts{ -1 }
        , lastDts{ -1 }
        , lastPcr{ -1 }
        , numberOfMissingPts{ 0 }
        , numberOfMissingDts{ 0 }
    {
    }

    uint8_t lastCC;
    uint64_t numberOfCCErrors;

    uint64_t numberOfTsDiscontinuities;

    int64_t lastPts;
    std::map<int64_t, uint64_t> ptsHistogram;

    int64_t lastDts;
    std::map<int64_t, uint64_t> dtsHistogram;

    int64_t lastPcr;
    std::map<int64_t, uint64_t> pcrHistogram;

    uint64_t numberOfMissingPts;

    uint64_t numberOfMissingDts;
};

/*!
 * @brief Count TS packets statistics
 */
struct TsCounters
{
    uint64_t mTsPacketCounter{ 0 };
    uint64_t mTsPacketNullPacketCounter{ 0 };
    uint64_t mTsPacketErrorIndicator{ 0 };
};


typedef std::function<void(const ByteVector& rawTable, PsiTable* table, int pid, void* hdl)> PsiCallBackFnc;
typedef std::function<void(const ByteVector& rawPes, const PesPacket& pes, int pid, void* hdl)> PesCallBackFnc;
typedef std::function<void(const uint8_t* packet, TsPacketInfo tsPacketInfo, void* hdl)> TsCallBackFnc;

/*!
 * @brief mpeg2ts API
 * @brief Demux ts packets into PSI and PES (plus TS pass through)
 */
class TsDemuxer
{
public:
    MPEG2TS_EXPORT explicit TsDemuxer();

    MPEG2TS_EXPORT ~TsDemuxer();

    // Make this object be non copyable because it holds a pointer
    TsDemuxer(const TsDemuxer&) = delete;
    const TsDemuxer& operator=(const TsDemuxer&) = delete;

    /*!
     * Demuxes a transport stream packed based on its Packet ID.
     * @param tsPacket The TS packet to demux.
     */
    MPEG2TS_EXPORT void demux(const uint8_t* tsPacket);

    /*!
     * Outputs to callback function when found a PSI table with PID.
     * @param pid The PID to filter PSI tables.
     * @param cb Returns a complete PSI table to this callback function.
     * @param hdl Custom handler
     */
    MPEG2TS_EXPORT void addPsiPid(int pid, PsiCallBackFnc cb, void* hdl);

    /*!
     * Returns a complete PES packet with PID.
     * @param pid The PID to filter PES packets.
     * @param cb  Callback when found a complete PES packet.
     * @param hdl Custom handler
     */
    MPEG2TS_EXPORT void addPesPid(int pid, PesCallBackFnc cb, void* hdl);

    /*!
     * Returns a complete TS packet filtered on PID.
     * @param pid The PID to filter TS packets.
     * @param cb Callback when found a TS packet.
     * @param hdl Custom handler
     */
    MPEG2TS_EXPORT void addTsPid(int pid, TsCallBackFnc cb, void* hdl);

    /*!
     * Returns statistics on parsed transport stream packets.
     * @return PidStatisticsType containing collected statistics for each pid on all demuxed
     * packets.
     */
    MPEG2TS_EXPORT PidStatisticsMap getPidStatistics() const;

    /*!
     * Returns counter statistics on parsed transport stream packets.
     * @return TsCounters
     */
    MPEG2TS_EXPORT TsCounters getTsCounters() const;

    /*!
     * Returns the version of this library.
     * @return version (ex: "0.3.0")
     */
    MPEG2TS_EXPORT std::string getMpeg2tsLibVersion() const;

    MPEG2TS_EXPORT unsigned getMpeg2tsLibVersionMajor() const;

    MPEG2TS_EXPORT unsigned getMpeg2tsLibVersionMinor() const;

    MPEG2TS_EXPORT unsigned getMpeg2tsLibVersionPatch() const;

    MPEG2TS_EXPORT unsigned getMpeg2tsLibVersionTweak() const;
    
    MPEG2TS_EXPORT void setOrigin(int64_t origin);
    MPEG2TS_EXPORT int64_t getOrigin(int pid);

protected:
    std::map<int, PsiCallBackFnc> mPsiCallbackMap;
    std::map<int, PesCallBackFnc> mPesCallbackMap;
    std::map<int, TsCallBackFnc> mTsCallbackMap;
    std::map<int, void*> mHandlers;

private:
    std::unique_ptr<TsParser> mParser;

    int64_t mOrigin = 0;
};


} // namespace mpeg2ts

#endif /* _MPEG2TS_H */
