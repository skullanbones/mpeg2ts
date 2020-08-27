/*****************************************************************
*
*    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
*
*    mpeg2ts - mpeg2ts h264parser
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
******************************************************************/
#include <getopt.h>
#include <vector>
#include <string>

// 3rd-party
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>

// Project files
// #include "mpeg2ts_version.h" // Returns current version of mpeg2ts library

// Private project files
#include "H264Parser.h"


enum // Define log instances. Default is 0 and is omitted from this enum.
{
    FileLog = 1
};

const char LOGFILE_NAME[]{ "h264parser.csv" };
int LOGFILE_MAXSIZE{ 100 * 1024 };
int LOGFILE_MAXNUMBEROF{ 10 };

const plog::Severity DEFAULT_LOG_LEVEL = plog::debug;

std::map<std::string, std::vector<int>> g_Options;
std::string g_InputFile;



void display_usage()
{
    printf("H264 parser command-line tool:\n");

    printf(
    "USAGE: ./h264parser [-h] [-v] [-l log-level] [-i file]\n");

    printf("Option Arguments:\n"
           "        -h [ --help ]        Print help messages\n"
           "        -v [ --version ]     Print library version\n"
           "        -l [ --log-level NONE|FATAL|ERROR|WARNING|INFO|DEBUG|VERBOSE] Choose "
           "what logs are filtered, both file and stdout, default: %s\n"
           "        -i [ --input FILE]   Use input file for parsing\n",
           plog::severityToString(DEFAULT_LOG_LEVEL));
}


static const char* optString = "i:l:h?v";

struct option longOpts[] = { { "input", 1, nullptr, 'i' },   { "log-level", 1, nullptr, 'l' },
                             { "help", 0, nullptr, 'h' },    { "version", 0, nullptr, 'v' }, 
                             { nullptr, 0, nullptr, 0 } };

int main(int argc, char** argv)
{
    // Initialize the logger
    /// Short macros list
    /// LOGV << "verbose";
    /// LOGD << "debug";
    /// LOGI << "info";
    /// LOGW << "warning";
    /// LOGE << "error";
    /// LOGF << "fatal";
    /// LOGN << "none";

    static plog::RollingFileAppender<plog::CsvFormatter> fileAppender(LOGFILE_NAME, LOGFILE_MAXSIZE, LOGFILE_MAXNUMBEROF); // Create the 1st appender.
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender; // Create the 2nd appender.
    plog::init(DEFAULT_LOG_LEVEL, &fileAppender).addAppender(&consoleAppender); // Initialize the
                                                                                // logger with the
                                                                                // both appenders.
    plog::init<FileLog>(DEFAULT_LOG_LEVEL, &fileAppender); // Initialize the 2nd logger instance.

    LOGD << "Starting parser of file";

    for (;;)
    {
        int opt;
        int optInd = -1;
        opt = getopt_long(argc, argv, optString, longOpts, &optInd);
        if (optInd == -1)
        {
            for (optInd = 0; longOpts[optInd].name; ++optInd)
            {
                if (longOpts[optInd].val == opt)
                {
                    LOGD << "optInd: " << optInd;
                    break;
                }
            }
            if (longOpts[optInd].name == NULL)
            {
                // the short option was not found; do something
                LOGE << "the short option was not found; do something"; // TODO
            }
        }

        if (opt < 0)
            break;
        switch (opt)
        {
        case 'h': /* fall-through is intentional */
        case '?':
        {
            display_usage();
            exit(EXIT_SUCCESS);
        }
        case 'v':
        {
            // printf("version: %s\n", getMpeg2tsVersion().c_str());
            exit(EXIT_SUCCESS);
        }
        case 'l':
        {
            LOGD << "Use Default log-level: " << plog::severityToString(DEFAULT_LOG_LEVEL);
            std::string logLevel = std::string(optarg);
            LOGD << "Got input log-level setting: " << logLevel;
            for (auto& c : logLevel)
            {
                c = static_cast<char>(toupper(c));
            }
            plog::Severity severity = plog::severityFromString(logLevel.c_str());
            plog::get()->setMaxSeverity(severity);
            LOGD << "Use log-level: " << plog::severityToString(severity) << ", (" << severity << ")";
            break;
        }
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
    } // for


    std::ifstream file(g_InputFile);
    std::vector<uint8_t> newVec;
    uint8_t byte;
    while(file >> byte){
        newVec.push_back(byte);
    }

    h264::H264EsParser h264Parser;
    std::vector<h264::EsInfoH264> infos = h264Parser.parse(newVec);

    for (auto info : infos)
    {
        LOGD << "----------------------------------------------";
        LOGD << "h264 nal type: " << h264::H264EsParser::toString(info.type);
        LOGD << "nal: " << h264::H264EsParser::toString(info.nalUnitType) << " " << info.msg;
        if (info.type == h264::H264InfoType::SliceHeader)
        {
            LOGD << info.slice.sliceTypeStr << ", pps id: " << info.pps.ppsId;
            if (info.slice.field)
            {
                LOGD << "field encoded: " << (info.slice.top ? " top" : " bottom");
            }
            else
            {
                LOGD << "frame encoded";
            }
        }
        else if (info.type == h264::H264InfoType::SequenceParameterSet)
        {
            LOGD << "sps id: " << info.pps.spsId << ", luma bits: " << info.sps.lumaBits
                 << ", chroma bits: " << info.sps.chromaBits << ", size: " << info.sps.width
                 << " x " << info.sps.height << ", ref pic: " << info.sps.numRefPics;
        }
        else if (info.type == h264::H264InfoType::PictureParameterSet)
        {
            LOGD << "sps id: " << info.pps.spsId << "pps id: " << info.pps.ppsId;
        }
    }
}
