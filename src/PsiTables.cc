#include "Logging.h"
#include <public/mpeg2ts.h>

/// 3rd-party
#include <plog/Log.h>

namespace mpeg2ts
{

std::ostream& operator<<(std::ostream& ss, const PsiTable& rhs)
{
    ss << std::endl << "-------------PsiTable------------- " << std::endl;
    ss << "table_id:" << std::hex << (int)rhs.table_id << std::dec << std::endl;
    ss << "section_syntax_indicator: " << (int)rhs.section_syntax_indicator << std::endl;
    ss << "section_length: " << (int)rhs.section_length << std::endl;
    ss << "transport_stream_id: " << (int)rhs.transport_stream_id << std::endl;
    ss << "version_number: " << (int)rhs.version_number << std::endl;
    ss << "current_next_indicator: " << (int)rhs.current_next_indicator << std::endl;
    ss << "section_number: " << (int)rhs.section_number << std::endl;
    ss << "last_section_number: " << (int)rhs.last_section_number << std::endl;
    return ss;
}

bool PsiTable::operator==(const PsiTable& rhs) const
{
    return CRC_32 == rhs.CRC_32 && table_id == rhs.table_id &&
           section_syntax_indicator == rhs.section_syntax_indicator &&
           section_length == rhs.section_length && transport_stream_id == rhs.transport_stream_id &&
           version_number == rhs.version_number && current_next_indicator == rhs.current_next_indicator &&
           section_number == rhs.section_number && last_section_number == rhs.last_section_number;
}

bool PsiTable::operator!=(const PsiTable& rhs) const
{
    return !operator==(rhs);
}

std::ostream& operator<<(std::ostream& ss, const PatTable& rhs)
{
    ss << std::endl << "-------------PatTable------------- " << std::endl;
    // ss << static_cast<const PsiTable&>(rhs) << std::endl;
    // ss << PsiTable::operator<<(rhs) << std::endl;
    ss << "programs.size(): " << (int)rhs.programs.size() << std::endl;
    for (unsigned int i = 0; i < rhs.programs.size(); i++)
    {
        ss << "-------------program " << i << "--------------" << std::endl;
        ss << "program_number: " << rhs.programs[i].program_number << std::endl;
        if (rhs.programs[i].type == ProgramType::PMT)
        {
            ss << "program_map_PID: " << rhs.programs[i].program_map_PID << std::endl;
        }
        else if (rhs.programs[i].type == ProgramType::NIT)
        {
            ss << "network_PID: " << rhs.programs[i].network_PID << std::endl;
        }
    }

    return ss;
}

bool PatTable::operator==(const PatTable& rhs) const
{
    bool psi = PsiTable::operator==(rhs);
    if (psi == false)
    {
        return false;
    }

    // 1. check number of programs
    if (this->programs.size() != rhs.programs.size())
    {
        LOGD_(FileLog) << "PatTable number of programs unequal." << std::endl;
        return false;
    }
    // 2. check content of each programs
    unsigned i = 0;
    for (auto prg : programs)
    {
        if (prg != rhs.programs.at(i))
        {
            LOGD_(FileLog)
            << "PatTable programs content unequal for program: " << prg.program_number << std::endl;
            return false;
        }
        i++;
    }
    return true;
}

bool PatTable::operator!=(const PatTable& rhs) const
{
    return !operator==(rhs);
}

std::ostream& operator<<(std::ostream& ss, const StreamTypeHeader& rhs)
{
    ss << std::endl << "-------------StreamTypeHeader------------- " << std::endl;
    ss << "stream_type: " << (int)rhs.stream_type << std::endl;
    ss << "elementary_PID: " << rhs.elementary_PID << std::endl;
    ss << "ES_info_length: " << rhs.ES_info_length << std::endl;
    return ss;
}

bool StreamTypeHeader::operator==(const StreamTypeHeader& rhs) const
{
    return stream_type == rhs.stream_type && elementary_PID == rhs.elementary_PID &&
           ES_info_length == rhs.ES_info_length;
}

bool StreamTypeHeader::operator!=(const StreamTypeHeader& rhs) const
{
    return !operator==(rhs);
}

/// PMT Table

std::ostream& operator<<(std::ostream& ss, const PmtTable& rhs)
{
    ss << std::endl << "-------------PmtTable------------- " << std::endl;
    ss << "PCR_PID: " << (int)rhs.PCR_PID << std::endl;
    ss << "program_info_length: " << (int)rhs.program_info_length << std::endl;
    ss << "streams.size(): " << (int)rhs.streams.size() << std::endl;
    for (unsigned int i = 0; i < rhs.streams.size(); i++)
    {
        ss << "-------------stream " << i << "--------------" << std::endl;
        ss << "stream_type: " << StreamTypeToString[(int)rhs.streams[i].stream_type] << ",  ("
           << (int)rhs.streams[i].stream_type << ")" << std::endl;
        ss << "elementary_PID: " << (int)rhs.streams[i].elementary_PID << std::endl;
        ss << "ES_info_length: " << (int)rhs.streams[i].ES_info_length << std::endl;
    }

    return ss;
}

bool PmtTable::operator==(const PmtTable& rhs) const
{
    bool psi = PsiTable::operator==(rhs);
    if (psi == false)
    {
        return false;
    }

    // 1. First check CRC 32
    if (this->CRC_32 != rhs.CRC_32)
    {
        LOGD_(FileLog) << "PmtTable CRC_32 unequal." << std::endl;
        return false;
    }
    // 2. Secondly check PCR_PID
    if (this->PCR_PID != rhs.PCR_PID)
    {
        LOGD_(FileLog) << "PmtTable PCR_PID unequal." << std::endl;
        return false;
    }
    // 3. Thirdly check program_info_length
    if (this->program_info_length != rhs.program_info_length)
    {
        LOGD_(FileLog) << "PmtTable program_info_length unequal." << std::endl;
        return false;
    }
    // 4. check number of streams
    if (this->streams.size() != rhs.streams.size())
    {
        LOGD_(FileLog) << "PmtTable number of streams unequal." << std::endl;
        return false;
    }
    // 5. check content of each streams
    unsigned i = 0;
    for (auto stream : streams)
    {
        if (stream != rhs.streams.at(i))
        {
            LOGD_(FileLog)
            << "PmtTable stream content unequal for stream_type: " << stream.stream_type << std::endl;
            return false;
        }
        i++;
    }
    return true;
}

bool PmtTable::operator!=(const PmtTable& rhs) const
{
    return !operator==(rhs);
}

} // namespace mpeg2ts