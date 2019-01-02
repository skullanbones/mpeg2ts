
#pragma once

#include <algorithm>
#include <cstdint>
#include <list>
#include <memory>
#include <vector>
#include <fstream>
#include <iostream>


#include "plog/Log.h"

struct EsInfo
{
    virtual ~EsInfo(){};
};

class EsParser
{
public:
    EsParser()
    {
        outdata.open("example1.dat");
    }

    virtual ~EsParser()
    {
        outdata.close();
    }

    std::vector<std::shared_ptr<EsInfo>> parse(std::vector<uint8_t> buf);
    std::vector<std::size_t> findStartCodes(std::vector<uint8_t> buf);

    virtual std::vector<std::shared_ptr<EsInfo>> analyze() = 0;

    std::vector<std::size_t> m_indexes{}; // indexes of 1st position
protected:
    std::vector<uint8_t> mPicture;
    std::ofstream outdata;
};

inline std::vector<std::size_t> EsParser::findStartCodes(std::vector<uint8_t> a_buf)
{
    std::vector<uint8_t> needle = {0x00, 0x00, 0x00, 0x01};
    std::vector<std::size_t> indexes{};
    auto it{a_buf.begin()};
    while ((it = std::search(it, a_buf.end(), needle.begin(), needle.end())) != a_buf.end()) 
    {
        indexes.push_back(std::distance(a_buf.begin(), it++));
    }
    m_indexes = indexes; // TODO fix this remove
    return indexes;
}

inline std::vector<std::shared_ptr<EsInfo>> EsParser::parse(std::vector<uint8_t> a_buf)
{
    std::vector<std::shared_ptr<EsInfo>> ret{};

    std::vector<std::size_t> startCodes = findStartCodes(a_buf);

    printf("found num startCodes: %d\n", static_cast<int>(startCodes.size()));
    
    // No startcodes -> no parsing
    if (startCodes.size() == 0)
        return ret;

    // There is nothing to parse if the frame only contains a NAL startcode
    if (a_buf.size() <= 4 )
        return ret;

    // Only have 1 startcode is a corner case
    if (startCodes.size() == 1 && a_buf.size() > 4)
    {
        try
        {
            mPicture.clear();
            std::vector<uint8_t>::const_iterator first = a_buf.begin() + startCodes[0] + 4; // skip start code
            std::vector<uint8_t>::const_iterator last = a_buf.end();             
            std::vector<uint8_t> newVec(first, last);
            mPicture = newVec;
        
            auto vec = analyze();
            for (auto& l : vec)
            {
                ret.push_back(l);
            }
        }
        catch (std::bad_alloc& e)
        {
            printf("std::Exception what: %s\n", e.what());
        }
        catch (...)
        {
            printf("exception e:\n");
        }
        return ret;
    }

    printf("startCode[0]: %d\n", static_cast<int>(startCodes[0]));
    printf("startCode[1]: %d\n", static_cast<int>(startCodes[1]));

    std::cout << "startCode[0]:" << static_cast<int>(startCodes[0]) << '\n';
    std::cout << "startCode[1]:" << static_cast<int>(startCodes[1]) << '\n';

    LOGD << "startCode[0]:" << static_cast<int>(startCodes[0]) << '\n';
    LOGD << "startCode[1]:" << static_cast<int>(startCodes[1]) << '\n';


    for (std::size_t ind = 0; ind < startCodes.size(); ++ind)
    {
        // create sub vector
        try
        {
            mPicture.clear();
            std::vector<uint8_t>::const_iterator first = a_buf.begin() + startCodes[ind] + 4; // skip start code
            std::vector<uint8_t>::const_iterator last;
            if (ind == (startCodes.size() - 1))
            {
                last = a_buf.end();
            }
            else {
                last = a_buf.begin() + startCodes[ind + 1];
            }
             
            std::vector<uint8_t> newVec(first, last);
            mPicture = newVec;
        
            auto vec = analyze();
            for (auto& l : vec)
            {
                ret.push_back(l);
            }
        }
        catch (std::bad_alloc& e)
        {
            printf("std::Exception what: %s\n", e.what());
        }
        catch (...)
        {
            printf("exception e:\n");
        }
        
    }

    return ret;
}
