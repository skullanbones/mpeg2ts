
#pragma once

#include <algorithm>
#include <cstdint>
#include <list>
#include <memory>
#include <vector>

struct EsInfo
{
    virtual ~EsInfo(){};
};

class EsParser
{
public:
    EsParser()
        : m_foundStartCodes{ 0 }
    {
    }

    virtual ~EsParser()
    {
    }

    const uint8_t* getFirstOne(const uint8_t* from, std::size_t length)
    {
        // TODO: avx2 can compare 32 bytes within one cycle
        //      and return pattern position using one more cycle
        return std::find(from, from + length, 1);
    }

    std::vector<std::shared_ptr<EsInfo>> parse(const uint8_t* from, std::size_t length);
    virtual std::vector<std::shared_ptr<EsInfo>> analyze() = 0;


    int m_foundStartCodes;
    std::vector<std::size_t> m_indexes{}; // indexes of 1st position
protected:
    std::vector<uint8_t> mPicture;
};

inline std::vector<std::shared_ptr<EsInfo>> EsParser::parse(const uint8_t* a_from, std::size_t a_length)
{
    std::vector<std::shared_ptr<EsInfo>> ret;

    while (a_length > 0)
    {
        const uint8_t* onePosition = getFirstOne(a_from, a_length);
        auto startCodeFound = false;
        if (onePosition == a_from)
        {
            if (mPicture.size() >= 2 && mPicture[mPicture.size() - 2] == 0 && mPicture[mPicture.size() - 1] == 0)
            {
                startCodeFound = true;
            }
        }
        else if (onePosition == a_from + 1)
        {
            if (mPicture.size() >= 1 && mPicture[mPicture.size() - 1] == 0 && *(onePosition - 1) == 0)
            {
                startCodeFound = true;
            }
        }
        else if (onePosition != a_from + a_length)
        {
            if (*(onePosition - 2) == 0 && *(onePosition - 1) == 0)
            {
                startCodeFound = true;
            }
        }
        const uint8_t* end = (onePosition != a_from + a_length) ? onePosition + 1 : onePosition;
        std::copy(a_from, end, std::back_inserter(mPicture));
        if (startCodeFound)
        {
            ++m_foundStartCodes;
             std::size_t ind = (onePosition - a_from);
            m_indexes.push_back(ind);
            
            if (mPicture.size() > 4)
            {
                auto vec = analyze();
                for (auto& l : vec)
                {
                    ret.push_back(l);
                }
            }
            mPicture = { 0, 0, 0, 1 };
        }
        std::size_t diff = (onePosition + 1 - a_from);
        a_length = diff > a_length ? 0 : a_length - diff;
        a_from = onePosition + 1;
    }

    return ret;
}