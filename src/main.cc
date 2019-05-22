commit ba40faec19ecb94e616648fc34ebb88da64f22e7
Author: workstation <lnwhome@interia.pl>
Date:   Sat May 18 01:50:31 2019 +0200

    generate big json

diff --git a/src/main.cc b/src/main.cc
index c68f7fb..b81a72b 100644
--- a/src/main.cc
+++ b/src/main.cc
@@ -18,6 +18,7 @@
 // 3rd-party
 #include <plog/Appenders/ConsoleAppender.h>
 #include <plog/Log.h>
+#include <nlohmann/json.hpp>
 
 // Project files
 #include "Logging.h"
@@ -37,6 +38,8 @@ std::vector<uint16_t> g_ESPIDS;
 mpeg2ts::TsDemuxer g_tsDemux;
 PatTable g_prevPat;
 std::map<int, PmtTable> g_prevPmts;
+bool g_Auto = false;
+nlohmann::json g_BigJson;
 bool addedPmts{ false };
 
 const char LOGFILE_NAME[]{ "tsparser.csv" };
@@ -86,7 +89,8 @@ void display_usage()
            "        -m [ --wrmode type]  Choose what type of data is written[ts|pes|es]\n"
            "        -l [ --log-level NONE|FATAL|ERROR|WARNING|INFO|DEBUG|VERBOSE] Choose "
            "what logs are filtered, both file and stdout, default: %s\n"
-           "        -i [ --input FILE]   Use input file for parsing\n",
+           "        -i [ --input FILE]   Use input file for parsing\n"
+           "        -a [ --auto ]   Output all as json\n",
            plog::severityToString(DEFAULT_LOG_LEVEL));
 }
 
@@ -154,6 +158,14 @@ void TsCallback(const uint8_t* packet, TsPacketInfo tsPacketInfo)
 void PATCallback(const ByteVector& /* rawPes*/, PsiTable* table, int pid)
 {
     LOGV << "PATCallback pid:" << pid;
+    
+    if (g_Auto)
+    {
+        nlohmann::json jsonPat;
+        jsonPat["ofs"] = g_tsDemux.getOrigin(pid);
+        g_BigJson["stream"]["Pid0"].push_back(jsonPat);
+    }
+    
     PatTable* pat;
     try
     {
@@ -184,6 +196,16 @@ void PATCallback(const ByteVector& /* rawPes*/, PsiTable* table, int pid)
         LOGN << "PAT at Ts packet: " << g_tsDemux.getTsCounters().mTsPacketCounter << '\n';
         LOGN << *pat << '\n';
     }
+    
+    if (g_Auto)
+    {
+        g_BigJson["stream"]["Pid0"].back()["pmtPids"] = nlohmann::json::array();
+    }
+
+    // new PAT clear everything
+    //TODO: clear pids from g_tsDemux. currently no API
+    g_PMTPIDS.clear();
+    addedPmts = false;
 
     // Check if MPTS or SPTS
     auto numPrograms = pat->programs.size();
@@ -196,6 +218,10 @@ void PATCallback(const ByteVector& /* rawPes*/, PsiTable* table, int pid)
     {
         LOGD << "Found Single Program Transport Stream (SPTS).";
         g_PMTPIDS.push_back(pat->programs[0].program_map_PID);
+        if (g_Auto)
+        {
+            g_BigJson["stream"]["Pid0"].back()["pmtPids"].push_back(pat->programs[0].program_map_PID);
+        }
     }
     else if (numPrograms >= 1) // MPTS
     {
@@ -205,6 +231,10 @@ void PATCallback(const ByteVector& /* rawPes*/, PsiTable* table, int pid)
             if (program.type == ProgramType::PMT)
             {
                 g_PMTPIDS.push_back(program.program_map_PID);
+                if (g_Auto)
+                {
+                    g_BigJson["stream"]["Pid0"].back()["pmtPids"].push_back(program.program_map_PID);
+                }
             }
         }
     }
@@ -217,6 +247,23 @@ void PMTCallback(const ByteVector& /* rawPes*/, PsiTable* table, int pid)
 {
     LOGV << "PMTCallback... pid:" << pid;
     PmtTable* pmt;
+    
+    if (g_Auto)
+    {
+        try
+        {
+            auto& pmtArray = g_BigJson["stream"].at("Pid" + std::to_string(pid));
+            nlohmann::json jsonPmt;
+            jsonPmt["ofs"] = g_tsDemux.getOrigin(pid);
+            pmtArray.push_back(jsonPmt);
+        }
+        catch(...)
+        {
+            nlohmann::json jsonPmt;
+            jsonPmt["ofs"] = g_tsDemux.getOrigin(pid);
+            g_BigJson["stream"]["Pid" + std::to_string(pid)] = nlohmann::json::array({jsonPmt});
+        }
+    }
 
     try
     {
@@ -236,7 +283,7 @@ void PMTCallback(const ByteVector& /* rawPes*/, PsiTable* table, int pid)
 
 
     // Do nothing if same PMT
-    if (g_prevPmts.find(pid) != g_prevPmts.end())
+    if (g_prevPmts.find(pid) != g_prevPmts.end()) //TODO: g_prevPmts.find(pid) != *pmt, right ???
     {
         LOGV << "Got same PMT...";
         return;
@@ -249,20 +296,33 @@ void PMTCallback(const ByteVector& /* rawPes*/, PsiTable* table, int pid)
         LOGN << "PMT at Ts packet: " << g_tsDemux.getTsCounters().mTsPacketCounter;
         LOGN << *pmt;
     }
+    
+    if (g_Auto)
+    {
+        g_BigJson["stream"]["Pid" + std::to_string(pid)].back()["streams"] = nlohmann::json::array();
+    }
 
     for (auto& stream : pmt->streams)
     {
         if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), stream.elementary_PID) ||
-            std::count(g_Options["write"].begin(), g_Options["write"].end(), stream.elementary_PID))
+            std::count(g_Options["write"].begin(), g_Options["write"].end(), stream.elementary_PID) || g_Auto)
         {
             LOGD << "Add ES PID: " << stream.elementary_PID << '\n';
             g_ESPIDS.push_back(stream.elementary_PID);
         }
+        
+        if (g_Auto)
+        {
+            nlohmann::json pmtEntry;
+            pmtEntry["pid"] = stream.elementary_PID;
+            pmtEntry["type"] = stream.stream_type;
+            g_BigJson["stream"]["Pid" + std::to_string(pid)].back()["streams"].push_back(pmtEntry);
+        }
     }
     if (pmt->PCR_PID != 0)
     {
         if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), pmt->PCR_PID) ||
-            std::count(g_Options["write"].begin(), g_Options["write"].end(), pmt->PCR_PID))
+            std::count(g_Options["write"].begin(), g_Options["write"].end(), pmt->PCR_PID) || g_Auto)
         {
             LOGD << "Add PCR PID: " << pmt->PCR_PID << '\n';
             g_ESPIDS.push_back(pmt->PCR_PID);
@@ -277,6 +337,24 @@ void PESCallback(const ByteVector& rawPes, const PesPacket& pes, int pid)
         LOGN << "PES ENDING at Ts packet " << g_tsDemux.getTsCounters().mTsPacketCounter << " (" << pid << ")\n";
         LOGN << pes << '\n';
     }
+    
+    if (g_Auto)
+    {
+        try
+        {
+            auto& pesArray = g_BigJson["stream"].at("Pid" + std::to_string(pid));
+            nlohmann::json jsonPes;
+            jsonPes["ofs"] = g_tsDemux.getOrigin(pid);
+            pesArray.push_back(jsonPes);
+        }
+        catch(...)
+        {
+            nlohmann::json jsonPes;
+            jsonPes["ofs"] = g_tsDemux.getOrigin(pid);
+            g_BigJson["stream"]["Pid" + std::to_string(pid)] = nlohmann::json::array({jsonPes});
+        }
+    }    
+    
     // @TODO add "if parse pid" option to cmd line
     {
         for (auto& pmtPid : g_PMTPIDS)
@@ -327,9 +405,12 @@ void PESCallback(const ByteVector& rawPes, const PesPacket& pes, int pid)
 
                             h264::H264EsParser h264Parser;
                             std::vector<h264::EsInfoH264> infos = h264Parser.parse(newVec);
-
+                            g_BigJson["stream"]["Pid" + std::to_string(pid)].back()["nals"] = nlohmann::json::array();
                             for (auto info : infos)
                             {
+                                nlohmann::json jsonh264Nal;
+                                jsonh264Nal["type"] = h264::H264EsParser::toString(info.type);
+                                g_BigJson["stream"]["Pid" + std::to_string(pid)].back()["nals"].push_back(jsonh264Nal);
                                 LOGD << "----------------------------------------------";
                                 LOGD << "h264 nal type: " << h264::H264EsParser::toString(info.type);
                                 LOGD << "nal: " << h264::H264EsParser::toString(info.nalUnitType)
@@ -413,12 +494,13 @@ extern void printTsPacket(const uint8_t* packet)
     printf("\n");
 }
 
-static const char* optString = "m:w:i:l:p:h?v";
+static const char* optString = "m:w:i:l:p:h?va";
 
 struct option longOpts[] = { { "write", 1, nullptr, 'w' },   { "wrmode", 1, nullptr, 'm' },
                              { "input", 1, nullptr, 'i' },   { "log-level", 1, nullptr, 'l' },
                              { "pid", 1, nullptr, 'p' },     { "help", 0, nullptr, 'h' },
-                             { "version", 0, nullptr, 'v' }, { nullptr, 0, nullptr, 0 } };
+                             { "help", 0, nullptr, '?' }, { "version", 0, nullptr, 'v' },
+                             { "auto", 0, nullptr, 'a' }, { nullptr, 0, nullptr, 0 } };
 
 int main(int argc, char** argv)
 {
@@ -459,7 +541,7 @@ int main(int argc, char** argv)
             if (longOpts[optInd].name == NULL)
             {
                 // the short option was not found; do something
-                LOGE << "the short option was not found; do something"; // TODO
+                LOGE << optInd << " the short option was not found; do something"; // TODO
             }
         }
 
@@ -527,6 +609,12 @@ int main(int argc, char** argv)
             g_InputFile = std::string(optarg);
             break;
         }
+        case 'a':
+        {
+            LOGD << "Got auto request";
+            g_Auto = true;
+            break;
+        }
         default:
             /* You won't actually get here. */
             break;
@@ -568,6 +656,10 @@ int main(int argc, char** argv)
     };
     g_tsDemux.addPsiPid(TS_PACKET_PID_PAT, f1, nullptr); // Find PAT
 
+    g_BigJson["stream"] = nlohmann::json::object();
+    g_BigJson["stream"]["name"] = g_InputFile;
+    g_BigJson["stream"]["Pid0"] = nlohmann::json::array();
+
     for (count = 0;; ++count)
     {
         unsigned char packet[TS_PACKET_SIZE + 1];
@@ -590,6 +682,9 @@ int main(int argc, char** argv)
                 LOGD << "Statistics\n";
                 display_statistics(g_tsDemux.getPidStatistics());
                 fclose(fptr);
+                
+                LOGE << g_BigJson.dump();
+                
                 return EXIT_SUCCESS;
             }
         }
@@ -597,6 +692,8 @@ int main(int argc, char** argv)
         // TS Packet start
         packet[0] = b;
 
+        auto dataOrigin = ftell(fptr) - 1;
+
         // Read TS Packet from file
         std::size_t res =
         fread(packet + 1, 1, TS_PACKET_SIZE, fptr); // Copy only packet size + next sync byte
@@ -621,6 +718,7 @@ int main(int argc, char** argv)
         try
         {
             parser.parseTsPacketInfo(packet, info);
+            g_tsDemux.setOrigin(dataOrigin);
             g_tsDemux.demux(packet);
         }
         catch (GetBitsException& e)
