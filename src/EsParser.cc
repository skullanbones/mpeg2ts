
#include <vector>
#include <memory>

#include "plog/Log.h"

#include "EsParser.h"

std::vector<std::shared_ptr<EsInfo>> EsParser::parse(const std::vector<uint8_t>& a_buf)
{
    std::vector<std::shared_ptr<EsInfo>> ret{};

    std::vector<std::size_t> startCodes = findStartCodes(a_buf);

    // No startcodes -> no parsing
    if (startCodes.size() == 0)
        return ret;

    // There is nothing to parse if the frame only contains a NAL startcode
    if (a_buf.size() <= m_startCode.size() )
        return ret;

    for (std::size_t ind = 0; ind < startCodes.size(); ++ind)
    {
        // create sub vector
        try
        {
            mPicture.clear();
            std::vector<uint8_t>::const_iterator first = a_buf.begin() + startCodes[ind] + m_startCode.size(); // skip start code
            std::vector<uint8_t>::const_iterator last;
            // the last startcode is a corner case
            // also only have 1 startcode is a corner case
            if (ind == (startCodes.size() - 1) || startCodes.size() == 1)
            {
                last = a_buf.end();
            }
            else {
                last = a_buf.begin() + startCodes[ind + 1];
            }
             
            std::vector<uint8_t> newVec(first, last);
            mPicture = newVec;
        
            analyze();
        }
        catch (std::bad_alloc& e)
        {
            LOGE << "std::Exception what: %s\n" << e.what();
        }
        
    }

    return ret;
}
