#include "public/TsUtilities.h"
#include <iostream>
#include "public/Ts_IEC13818-1.h"

namespace tsutil
{

bool TsUtilities::parseTransportStreamData(const uint8_t* data, std::size_t size)
{
    //_addedPmts = false;
    mPmtPids.clear();  // Restart

    // If empty data, just return
    if (data == NULL || data == nullptr)
    {
        std::cerr << "No data to parse..." << std::endl;
        return false;
    }

    uint64_t count = 0;
    int readIndex = 0;

    // Register PAT callback
    mDemuxer.addPsiPid(TS_PACKET_PID_PAT, std::bind(&PATCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), (void*) this);

    if ((data[0] != 0x47) || (size <= 0)) // TODO support maxsize?
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
    TsUtilities* instance = reinterpret_cast<TsUtilities*>(hdl);
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
        instance->mPmtPids.push_back(pat->programs[0].program_map_PID);
    }
    else if (numPrograms >= 1) // MPTS
    {
        //LOGD << "Found Multiple Program Transport Stream (MPTS).";
        for (auto program : pat->programs)
        {
            if (program.type == ProgramType::PMT)
            {
                instance->mPmtPids.push_back(program.program_map_PID);
            }
        }
    }


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

} // namespace tsutil
