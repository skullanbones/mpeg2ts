#include "public/TsUtilities.h"
#include <iostream>
#include "public/Ts_IEC13818-1.h"

#include <fstream>

namespace tsutil
{
TsUtilities::TsUtilities()
    : mAddedPmts{ false }
{
    
}


void TsUtilities::initParse()
{
    mPmtPids.clear();  // Restart
    mPrevPat = {};
    mPmts.clear();
    // Register PAT callback
    mDemuxer.addPsiPid(TS_PACKET_PID_PAT, std::bind(&PATCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), (void*) this);
}

void TsUtilities::registerPmtCallback()
{
    if (!mAddedPmts && !mPmtPids.empty())
    {
        for (auto pid : mPmtPids)
        {
            //LOGD << "Adding PSI PID for parsing: " << pid;
            mDemuxer.addPsiPid(pid, std::bind(&PMTCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), (void*) this);
        }
        mAddedPmts = true;
    }
}


bool TsUtilities::parseTransportFile(const std::string& file)
{
    initParse();
    std::string line;
    std::ifstream tsFile(file, std::ifstream::binary);

    if (!tsFile) {
        return false;
    }

    std::cout << "Parsing tsfile:" << file << std::endl;

    while (!tsFile.eof())
    {
        uint8_t packet[188];
        tsFile.read((char*)packet, 188); // TODO check sync byte
        mDemuxer.demux(packet);
    }
    tsFile.close();

    return true;
}


bool TsUtilities::parseTransportStreamData(const uint8_t* data, std::size_t size)
{
    //_addedPmts = false;
    initParse();

    // If empty data, just return
    if (data == NULL || data == nullptr)
    {
        std::cerr << "No data to parse..." << std::endl;
        return false;
    }

    uint64_t count = 0;
    uint64_t readIndex = 0;



    if ((data[0] != TS_PACKET_SYNC_BYTE) || (size <= 0)) // TODO support maxsize?
    {
        std::cerr << "ERROR: 1'st byte not in sync!!!" << std::endl;
        return false;
    }

    while (readIndex < size)
    {
        mDemuxer.demux(data + readIndex);
        readIndex += TS_PACKET_SIZE;
        count++;
    }
    std::cout << "Found " << count << " ts packets." << std::endl;

    return true;
}


void TsUtilities::PATCallback(PsiTable* table, uint16_t pid, void* hdl)
{
    auto instance = reinterpret_cast<TsUtilities*>(hdl); // TODO try/catch
    //LOGV << "PATCallback pid:" << pid;
    PatTable* pat;
    try
    {
        pat = dynamic_cast<PatTable*>(table);
    }
    catch (std::exception& ex)
    {
        //LOGE_(FileLog) << "ERROR: dynamic_cast ex: " << ex.what();
        return;
    }

    if (pat == NULL)
    {
        //LOGE_(FileLog) << "ERROR: This should not happen. You have some corrupt stream!!!";
        return;
    }

    // Do nothing if same PAT
    if (instance->mPrevPat == *pat)
    {
        //LOGV << "Got same PAT...";
        return;
    }
    instance->mPrevPat = *pat;


    // Check if MPTS or SPTS
    int numPrograms = pat->programs.size();
    if (numPrograms == 0)
    {
        //LOGD << "No programs found in PAT, exiting...";
        exit(EXIT_SUCCESS);
    }
    else if (numPrograms == 1) // SPTS
    {
        //LOGD << "Found Single Program Transport Stream (SPTS).";
        std::cout << "Found Single Program Transport Stream (SPTS)." << std::endl;
        instance->mPmtPids.push_back(pat->programs[0].program_map_PID);
    }
    else if (numPrograms >= 1) // MPTS
    {
        //LOGD << "Found Multiple Program Transport Stream (MPTS).";
        std::cout << "Found Multiple Program Transport Stream (MPTS)." << std::endl;
        for (auto program : pat->programs)
        {
            if (program.type == ProgramType::PMT)
            {
                instance->mPmtPids.push_back(program.program_map_PID);
            }
        }
    }

    instance->registerPmtCallback();

    // TODO: add writing of table
}

PatTable TsUtilities::getPatTable() const
{
    return mPrevPat;
}

std::vector<uint16_t> TsUtilities::getPmtPids() const
{
    return mPmtPids;
}

void TsUtilities::PMTCallback(PsiTable* table, uint16_t pid, void* hdl)
{
    auto instance = reinterpret_cast<TsUtilities*>(hdl);

    //LOGV << "PMTCallback... pid:" << pid;
    PmtTable* pmt;

    try
    {
        pmt = dynamic_cast<PmtTable*>(table);
    }
    catch (std::exception& ex)
    {
        //LOGE_(FileLog) << "ERROR: dynamic_cast ex: " << ex.what();
        return;
    }

    if (pmt == NULL)
    {
        //LOGE_(FileLog) << "ERROR: This should not happen. You have some corrupt stream!!!";
        return;
    }


    // Do nothing if same PMT
    if (instance->mPmts.find(pid) != instance->mPmts.end())
    {
        //LOGV << "Got same PMT...";
        return;
    }

    std::cout << "Adding PMT to list..." << std::endl;
    instance->mPmts[pid] = *pmt;
    
    for (auto& stream : pmt->streams)
    {
            //LOGD << "Add ES PID: " << stream.elementary_PID << std::endl;
        instance->mEsPids.push_back(stream.elementary_PID);
    }
    /*
    if (pmt->PCR_PID != 0)
    {
        if (std::count(g_Options["pid"].begin(), g_Options["pid"].end(), pmt->PCR_PID) ||
            std::count(g_Options["write"].begin(), g_Options["write"].end(), pmt->PCR_PID))
        {
            //LOGD << "Add PCR PID: " << pmt->PCR_PID << std::endl;
            instance->mEsPids.push_back(pmt->PCR_PID);
        }
    }*/
}

std::map<uint16_t, PmtTable> TsUtilities::getPmtTables() const
{
    return mPmts;
}


} // namespace tsutil
