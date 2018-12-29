
#include "Logging.h"
#include "public/mpeg2ts.h"

/// 3rd-party
#include <plog/Log.h>

namespace mpeg2ts
{

std::ostream& operator<<(std::ostream& ss, const PsiTable& rhs)
{
    ss << '\n' << "-------------PsiTable-------------" << '\n';
    ss << "table_id:" << std::hex << static_cast<int>(rhs.table_id) << std::dec << '\n';
    ss << "section_syntax_indicator: " << static_cast<int>(rhs.section_syntax_indicator) << '\n';
    ss << "section_length: " << static_cast<int>(rhs.section_length) << '\n';
    ss << "transport_stream_id: " << static_cast<int>(rhs.transport_stream_id) << '\n';
    ss << "version_number: " << static_cast<int>(rhs.version_number) << '\n';
    ss << "current_next_indicator: " << static_cast<int>(rhs.current_next_indicator) << '\n';
    ss << "section_number: " << static_cast<int>(rhs.section_number) << '\n';
    ss << "last_section_number: " << static_cast<int>(rhs.last_section_number) << '\n';
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
    ss << '\n' << "-------------PatTable-------------" << '\n';
    // ss << static_cast<const PsiTable&>(rhs) << '\n';
    // ss << PsiTable::operator<<(rhs) << '\n';
    ss << "programs.size(): " << static_cast<int>(rhs.programs.size()) << '\n';
    for (unsigned int i = 0; i < rhs.programs.size(); ++i)
    {
        ss << "-------------program " << i << "--------------" << '\n';
        ss << "program_number: " << rhs.programs[i].program_number << '\n';
        if (rhs.programs[i].type == ProgramType::PMT)
        {
            ss << "program_map_PID: " << rhs.programs[i].program_map_PID << '\n';
        }
        else if (rhs.programs[i].type == ProgramType::NIT)
        {
            ss << "network_PID: " << rhs.programs[i].network_PID << '\n';
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
        LOGD_(FileLog) << "PatTable number of programs unequal." << '\n';
        return false;
    }
    // 2. check content of each programs
    unsigned i = 0;
    for (auto prg : programs)
    {
        if (prg != rhs.programs.at(i))
        {
            LOGD_(FileLog)
            << "PatTable programs content unequal for program: " << prg.program_number << '\n';
            return false;
        }
        ++i;
    }
    return true;
}

bool PatTable::operator!=(const PatTable& rhs) const
{
    return !operator==(rhs);
}

std::ostream& operator<<(std::ostream& ss, const StreamTypeHeader& rhs)
{
    ss << '\n' << "-------------StreamTypeHeader-------------" << '\n';
    ss << "stream_type: " << StreamTypeToString[static_cast<int>(rhs.stream_type)] << ",  ("
       << static_cast<int>(rhs.stream_type) << ")" << '\n';
    ss << "elementary_PID: " << rhs.elementary_PID << '\n';
    ss << "ES_info_length: " << rhs.ES_info_length << '\n';
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
    ss << '\n' << "-------------PmtTable-------------" << '\n';
    ss << "PCR_PID: " << static_cast<int>(rhs.PCR_PID) << '\n';
    ss << "program_info_length: " << static_cast<int>(rhs.program_info_length) << '\n';
    ss << "streams.size(): " << static_cast<int>(rhs.streams.size()) << '\n';
    for (std::size_t i = 0; i < rhs.streams.size(); ++i)
    {
        ss << "-------------stream " << i << "--------------" << '\n';
        ss << "stream_type: " << StreamTypeToString[static_cast<int>(rhs.streams[i].stream_type)]
           << ",  (" << static_cast<int>(rhs.streams[i].stream_type) << ")" << '\n';
        ss << "elementary_PID: " << static_cast<int>(rhs.streams[i].elementary_PID) << '\n';
        ss << "ES_info_length: " << static_cast<int>(rhs.streams[i].ES_info_length) << '\n';
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
        LOGD_(FileLog) << "PmtTable CRC_32 unequal." << '\n';
        return false;
    }
    // 2. Secondly check PCR_PID
    if (this->PCR_PID != rhs.PCR_PID)
    {
        LOGD_(FileLog) << "PmtTable PCR_PID unequal." << '\n';
        return false;
    }
    // 3. Thirdly check program_info_length
    if (this->program_info_length != rhs.program_info_length)
    {
        LOGD_(FileLog) << "PmtTable program_info_length unequal." << '\n';
        return false;
    }
    // 4. check number of streams
    if (this->streams.size() != rhs.streams.size())
    {
        LOGD_(FileLog) << "PmtTable number of streams unequal." << '\n';
        return false;
    }
    // 5. check content of each streams
    unsigned i = 0;
    for (auto stream : streams)
    {
        if (stream != rhs.streams.at(i))
        {
            LOGD_(FileLog)
            << "PmtTable stream content unequal for stream_type: " << stream.stream_type << '\n';
            return false;
        }
        ++i;
    }
    return true;
}

bool PmtTable::operator!=(const PmtTable& rhs) const
{
    return !operator==(rhs);
}

} // namespace mpeg2ts