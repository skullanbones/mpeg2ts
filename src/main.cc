/**
 * Strictly Confidential - Do not duplicate or distribute without written
 * permission from authors
 */
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstdlib> // EXIT_SUCCESS
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <type_traits>
#include <unistd.h>

/// 3rd-party
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

/// Project files
#include "TsDemuxer.h"
#include "TsPacketInfo.h"
#include "TsStandards.h"
#include "Logging.h"

class EsParser
{
    public:
    EsParser()
    {
    
    }
    virtual ~EsParser()
    {
    
    }

    virtual bool operator()(const uint8_t* from, size_t length)
    {
        LOGD << from << length;
        return true;
    }
};

class Mpeg2VideoEsParser : public EsParser
{
    public:
    Mpeg2VideoEsParser()
    {
    
    }
    virtual ~Mpeg2VideoEsParser()
    {
    
    }

    virtual bool operator()(const uint8_t* from, size_t length)
    {
        // TODO: parse data
        LOGD << "mpeg2v " << from << length;
        return true;
    }
};


static const std::string VERSION = "0.0.2.rc1";

uint64_t count = 0;
uint64_t countAdaptPacket = 0;
std::vector<uint16_t> g_PMTPIDS;
std::vector<uint16_t> g_ESPIDS;
TsDemuxer g_tsDemux;
PatTable g_prevPat;
std::map<uint16_t, PmtTable> g_prevPmts;
bool addedPmts = false;
std::map<StreamType, std::shared_ptr<EsParser> > g_EsParsers = {{STREAMTYPE_VIDEO_MPEG2, std::make_shared<Mpeg2VideoEsParser>()}};

const char LOGFILE_NAME[] = "tsparser.csv";
int LOGFILE_MAXSIZE = 100 * 1024;
int LOGFILE_MAXNUMBEROF = 10;

const plog::Severity DEFAULT_LOG_LEVEL = plog::debug;

enum class OptionWriteMode
{
    TS,
    PES,
    ES
};

std::map<std::string, std::vector<int>> g_Options;
std::list<OptionWriteMode> g_WriteMode;
std::string g_InputFile;

static const char* optString = "m:w:i:l:p:h?v";

struct option longOpts[] = { { "write", 1, nullptr, 'w' }, { "wrmode", 1, nullptr, 'm' },
                             { "pid", 1, nullptr, 'p' },   { "log-level", 1, nullptr, 'l' },
                             { "input", 1, nullptr, 'i' }, { "help", 0, nullptr, 'h' },
                             { "version", 0, nullptr, 'v' }, { nullptr, 0, nullptr, 0 } };

bool hasPid(std::string param, uint32_t pid)
{
    return std::count(g_Options[param].begin(), g_Options[param].end(), pid);
}

bool hasPids(std::string param, std::vector<uint16_t> pids)
{
    bool ret = 0;
    for (auto pid : pids)
    {
        ret += std::count(g_Options[param].begin(), g_Options[param].end(), pid);
    }
    return ret;
}

void display_usage()
{
    std::cout << "Ts-lib simple command-line:" << std::endl;

    std::cout << "USAGE: ./tsparser [-h] [-v] [-p PID] [-w PID] [-m ts|pes|es] [-l log-level] [-i file]" << std::endl;

    std::cout << "Option Arguments:\n"
                 "        -h [ --help ]        Print help messages\n"
                 "        -v [ --version ]     Print library version\n"
                 "        -p [ --pid PID]      Print PSI tables info with PID\n"
                 "        -w [ --write PID]    Writes PES packets with PID to file\n"
                 "        -m [ --wrmode type]  Choose what type of data is written[ts|pes|es]\n"
                 "        -l [ --log-level NONE|FATAL|ERROR|WARNING|INFO|DEBUG|VERBOSE] Choose what logs are filtered, both file and stdout, default:" << plog::severityToString(DEFAULT_LOG_LEVEL) << "\n"
                 "        -i [ --input FILE]   Use input file for parsing"
              << std::endl;
}

void display_statistics(TsDemuxer demuxer)
{
    for (auto& pidStat : demuxer.getTsStatistics().mPidStatistics)
    {
        if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), pidStat.first) == 0)
        {
            continue;
        }
        LOGD << "Pid: " << pidStat.first << "\n";
        LOGD << " Transport Stream Discontinuity: " << pidStat.second.numberOfTsDiscontinuities << "\n";
        LOGD << " CC error: " << pidStat.second.numberOfCCErrors << "\n";
        LOGD << " Pts differences histogram:\n";
        for (auto& ent : pidStat.second.ptsHistogram)
        {
            LOGD << "  diff: " << ent.first << " quantity " << ent.second << "\n";
        }
        LOGD << " Pts missing: " << pidStat.second.numberOfMissingPts << "\n";

        LOGD << " Dts differences histogram:\n";
        for (auto& ent : pidStat.second.dtsHistogram)
        {
            LOGD << "  diff: " << ent.first << " quantity " << ent.second << "\n";
        }
        LOGD << " Dts missing: " << pidStat.second.numberOfMissingDts << "\n";
        LOGD << " Pcr differences histogram:\n";
        for (auto& ent : pidStat.second.pcrHistogram)
        {
            LOGD << "  diff: " << ent.first << " quantity " << ent.second << "\n";
        }
    }
}


void TsCallback(const uint8_t* packet, TsPacketInfo tsPacketInfo)
{
    auto pid = tsPacketInfo.pid;
    LOGD << "demuxed TS packet \n" << tsPacketInfo;
    if (hasPid("pid", pid))
    {
        LOGN << tsPacketInfo;
    }

    if (hasPid("write", pid))
    {
        static std::map<uint16_t, std::ofstream> outFiles;
        auto fit = outFiles.find(pid);
        if (fit == outFiles.end())
        {
            outFiles[pid] = std::ofstream(std::to_string(pid) + ".ts",
                                          std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        }
        if (g_WriteMode.front() != OptionWriteMode::TS)
        {
            return;
        }
        std::copy(packet, packet + TS_PACKET_SIZE, std::ostreambuf_iterator<char>(outFiles[pid]));

        LOGD << "Write TS: " << TS_PACKET_SIZE << " bytes, pid: " << pid << std::endl;
    }
}

void PATCallback(PsiTable* table, uint16_t pid)
{
    LOGV << "PATCallback pid:" << pid;
    PatTable* pat;
    try
    {
        pat = dynamic_cast<PatTable*>(table);
    }
    catch (std::exception& ex)
    {
        LOGE_(FileLog) << "ERROR: dynamic_cast ex: " << ex.what();
        return;
    }

    if (pat == NULL)
    {
        LOGE_(FileLog) << "ERROR: This should not happen. You have some corrupt stream!!!";
        return;
    }

    // Do nothing if same PAT
    if (g_prevPat == *pat)
    {
        LOGV << "Got same PAT...";
        return;
    }
    g_prevPat = *pat;

    if (hasPid("pid", TS_PACKET_PID_PAT))
    {
        LOGN << "PAT at Ts packet: " << g_tsDemux.getTsStatistics().mTsPacketCounter << "\n";
        LOGN << *pat << std::endl;
    }

    // Check if MPTS or SPTS
    int numPrograms = pat->programs.size();
    if (numPrograms == 0)
    {
        LOGD << "No programs found in PAT, exiting...";
        exit(EXIT_SUCCESS);
    }
    else if (numPrograms == 1) // SPTS
    {
        LOGD << "Found Single Program Transport Stream (SPTS).";
        g_PMTPIDS.push_back(pat->programs[0].program_map_PID);
    }
    else if (numPrograms >= 1) // MPTS
    {
        LOGD << "Found Multiple Program Transport Stream (MPTS).";
        for (auto program : pat->programs)
        {
            if (program.type == ProgramType::PMT)
            {
                g_PMTPIDS.push_back(program.program_map_PID);
            }
        }
    }


    // TODO: add writing of table
}

void PMTCallback(PsiTable* table, uint16_t pid)
{
    LOGV << "PMTCallback... pid:" << pid;
    PmtTable* pmt;

    try
    {
        pmt = dynamic_cast<PmtTable*>(table);
    }
    catch (std::exception& ex)
    {
        LOGE_(FileLog) << "ERROR: dynamic_cast ex: " << ex.what();
        return;
    }

    if (pmt == NULL)
    {
        LOGE_(FileLog) << "ERROR: This should not happen. You have some corrupt stream!!!";
        return;
    }


    // Do nothing if same PMT
    if (g_prevPmts.find(pid) != g_prevPmts.end())
    {
        LOGV << "Got same PMT...";
        return;
    }

    g_prevPmts[pid] = *pmt;

    if (hasPids("pid", g_PMTPIDS))
    {
        LOGN << "PMT at Ts packet: " << g_tsDemux.getTsStatistics().mTsPacketCounter;
        LOGN << *pmt;
    }

    for (auto& stream : pmt->streams)
    {
        if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), stream.elementary_PID) ||
            std::count(g_Options["write"].begin(), g_Options["write"].end(), stream.elementary_PID))
        {
            LOGD << "Add ES PID: " << stream.elementary_PID << std::endl;
            g_ESPIDS.push_back(stream.elementary_PID);
        }
    }
    if (pmt->PCR_PID != 0)
    {
        if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), pmt->PCR_PID) ||
            std::count(g_Options["write"].begin(), g_Options["write"].end(), pmt->PCR_PID))
        {
            LOGD << "Add PCR PID: " << pmt->PCR_PID << std::endl;
            g_ESPIDS.push_back(pmt->PCR_PID);
        }
    }
}

void PESCallback(const PesPacket& pes, uint16_t pid)
{

    if (hasPid("pid", pid))
    {
        LOGN << "PES ENDING at Ts packet " << g_tsDemux.getTsStatistics().mTsPacketCounter
                  << " (" << pid << ")\n";
        LOGN << pes << std::endl;
    }
    
    // @TODO add "if parse pid" option to cmd line
    {
        for (auto& pmtPid : g_PMTPIDS)
        {
            if (g_prevPmts.find(pmtPid) != g_prevPmts.end())
            {
                auto it = std::find_if(g_prevPmts[pmtPid].streams.begin(), g_prevPmts[pmtPid].streams.end(), [&](StreamTypeHeader& stream){return stream.elementary_PID == pid;});
                if (it != g_prevPmts[pmtPid].streams.end())
                {
                    try
                    {
                        (*g_EsParsers.at(it->stream_type))(&pes.mPesBuffer[pes.elementary_data_offset], pes.mPesBuffer.size() - pes.elementary_data_offset);
                    }catch(const std::out_of_range&){
                        LOGD << "No parser for stream type " << StreamTypeToString[it->stream_type];
                    }
                }
            }
        }
    }
    
    if (hasPid("write", pid))
    {
        auto writeOffset = 0;
        auto writeModeString = "";
        if (g_WriteMode.front() == OptionWriteMode::TS)
        {
            return;
        }
        else if (g_WriteMode.front() == OptionWriteMode::PES)
        {
            writeOffset = 0;
            writeModeString = "PES";
        }
        else
        {
            writeOffset = pes.elementary_data_offset;
            writeModeString = "ES";
        }

        static std::map<uint16_t, std::ofstream> outFiles;
        auto fit = outFiles.find(pid);
        if (fit == outFiles.end())
        {
            outFiles[pid] = std::ofstream(std::to_string(pid) + "." + writeModeString,
                                          std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        }

        std::copy(pes.mPesBuffer.begin() + writeOffset, pes.mPesBuffer.end(),
                  std::ostreambuf_iterator<char>(outFiles[pid]));

        LOGD << "Write " << writeModeString << ": " << pes.mPesBuffer.size() - writeOffset
                  << " bytes, pid: " << pid << std::endl;
    }
}

extern void printTsPacket(const uint8_t* packet)
{
    for (int i = 0; i < 188; i++)
    {
        printf ("0x%02x\n", packet[i]);
    }
    printf ("\n");
}

int main(int argc, char** argv)
{
    // Initialize the logger
    /// Short macros list
    ///LOGV << "verbose";
    ///LOGD << "debug";
    ///LOGI << "info";
    ///LOGW << "warning";
    ///LOGE << "error";
    ///LOGF << "fatal";
    ///LOGN << "none";

    static plog::RollingFileAppender<plog::CsvFormatter> fileAppender(LOGFILE_NAME, LOGFILE_MAXSIZE, LOGFILE_MAXNUMBEROF); // Create the 1st appender.
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender; // Create the 2nd appender.
    plog::init(DEFAULT_LOG_LEVEL, &fileAppender).addAppender(&consoleAppender); // Initialize the logger with the both appenders.
    plog::init<FileLog>(DEFAULT_LOG_LEVEL, &fileAppender); // Initialize the 2nd logger instance.

    LOGD << "Starting parser of file";

    int longIndex;

    int opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
    while (opt != -1)
    {
        switch (opt)
        {
        case 'h': /* fall-through is intentional */
        case '?':
        {
            display_usage();
            exit(EXIT_SUCCESS);
            break;
        }
        case 'v':
        {
            std::cout << "version: " << VERSION << std::endl;
            exit(EXIT_SUCCESS);
            break;
        }
        case 'w':
        case 'p':
            LOGD << "Got pid listener pid:" << std::atoi(optarg);
            g_Options[longOpts[longIndex].name].push_back(std::atoi(optarg));
            break;
        case 'l':
        {
            LOGD << "Use Default log-level: " << plog::severityToString(DEFAULT_LOG_LEVEL);
            std::string logLevel = std::string(optarg);
            LOGD << "Got input log-level setting: " << logLevel;
            for (auto & c: logLevel) c = toupper(c);
            plog::Severity severity = plog::severityFromString(logLevel.c_str());
            plog::get()->setMaxSeverity(severity);
            LOGD << "Use log-level: " << plog::severityToString(severity) << ", (" << severity << ")";
            break;
        }
        case 'm':
        {
            OptionWriteMode writeMode = OptionWriteMode::PES;
            if (std::string(optarg) == "ts")
            {
                writeMode = OptionWriteMode::TS;
            }
            else if (std::string(optarg) == "pes")
            {
                writeMode = OptionWriteMode::PES;
            }
            else if (std::string(optarg) == "es")
            {
                writeMode = OptionWriteMode::ES;
            }
            else
            {
                std::cerr << "Allowed values for write mode are: ts, pes, es";
                display_usage();
                exit(EXIT_FAILURE);
            }
            g_WriteMode.push_back(writeMode);
        }
        break;
        case 'i':
        {
            LOGD << "Got file input: " << std::string(optarg);
            g_InputFile = std::string(optarg);
            break;
        }
        default:
            /* You won't actually get here. */
            break;
        }

        opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
    }
    if (g_WriteMode.empty())
    {
        g_WriteMode.push_back(OptionWriteMode::PES);
    }

    if (g_WriteMode.front() == OptionWriteMode::TS)
    {
        for (auto pid : g_Options["write"])
        {
            g_tsDemux.addTsPid(pid, std::bind(&TsCallback, std::placeholders::_1, std::placeholders::_2), nullptr);
        }
    }

    uint64_t count;

    // FILE
    FILE* fptr;
    fptr = fopen(g_InputFile.c_str(), "rb");

    if (fptr == NULL)
    {
        LOGE << "ERROR: Invalid file! Exiting...";
        exit(EXIT_FAILURE);
    }

    // Find PAT
    g_tsDemux.addPsiPid(TS_PACKET_PID_PAT, std::bind(&PATCallback, std::placeholders::_1, std::placeholders::_2), nullptr);

    for (count = 0;; ++count)
    {
        unsigned char packet[TS_PACKET_SIZE];
        // SYNC
        // Check for the sync byte. When found start a new ts-packet parser...
        char b;

        b = fgetc(fptr);
        while (b != TS_PACKET_SYNC_BYTE)
        {
            // std::cout << "ERROR: Sync error!!!" << std::endl;
            b = fgetc(fptr);
            int eof = feof(fptr);
            if (eof != 0)
            {
                LOGD << "End Of File..." << std::endl;
                LOGD << "Found " << count << " ts-packets." << std::endl;
                LOGD << "Found Adaptation Field packets:" << countAdaptPacket << " ts-packets." << std::endl;

                LOGD << "Statistics\n";
                display_statistics(g_tsDemux);
                fclose(fptr);
                return EXIT_SUCCESS;
            }
        }

        // TS Packet start
        packet[0] = b;

        // Read TS Packet from file
        size_t res =
        fread(packet + 1, 1, TS_PACKET_SIZE, fptr); // Copy only packet size + next sync byte
        if (res != TS_PACKET_SIZE)
        {
            LOGE_(FileLog) << "ERROR: Could not read a complete TS-Packet, read: " << res; // May be last packet end of file.
        }
        //printTsPacket(packet);
        // TODO fix this. We are almost always in here where we dont have 2 consecutive synced
        // packets...
        if (packet[TS_PACKET_SIZE] != TS_PACKET_SYNC_BYTE)
        {
            LOGE << "ERROR: Ts-packet Sync error. Next packet sync: ";
            // (int)packet[TS_PACKET_SIZE] << std::endl;  fseek(fptr, -TS_PACKET_SIZE, SEEK_CUR);
            // continue; // Skip this packet since it's not synced.
        }
        fseek(fptr, -1, SEEK_CUR); // reset file pointer and skip sync after packet

        TsPacketInfo info;
        TsParser parser;
        try
        {
            parser.parseTsPacketInfo(packet, info);
            g_tsDemux.demux(packet);
        }
        catch (GetBitsException& e)
        {
            LOGE_(FileLog) << "Got exception: " << e.what();
            LOGE_(FileLog) << "Got header: " << info.hdr;
            LOGE_(FileLog) << "Got packet: " << info;
            fclose(fptr);
            exit(EXIT_FAILURE);
        }
        if (!addedPmts && (g_PMTPIDS.size() != 0u))
        {
            for (auto pid : g_PMTPIDS)
            {
                LOGD << "Adding PSI PID for parsing: " << pid;
                g_tsDemux.addPsiPid(pid, std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2), nullptr);
            }
            addedPmts = true;
        }

        for (auto pid : g_ESPIDS)
        {
            LOGD << "Adding PES PID for parsing: " << pid;
            g_tsDemux.addPesPid(pid, std::bind(&PESCallback, std::placeholders::_1, std::placeholders::_2), nullptr);
        }
        g_ESPIDS.clear();

    } // for loop
}
