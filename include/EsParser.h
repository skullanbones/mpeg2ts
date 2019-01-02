
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

    std::vector<std::shared_ptr<EsInfo>> parse(std::vector<uint8_t> buf);
    virtual std::vector<std::shared_ptr<EsInfo>> analyze() = 0;


    int m_foundStartCodes;
    std::vector<std::size_t> m_indexes{}; // indexes of 1st position
protected:
    std::vector<uint8_t> mPicture;
};

inline std::vector<std::shared_ptr<EsInfo>> EsParser::parse(std::vector<uint8_t> a_buf)
{
    std::vector<std::shared_ptr<EsInfo>> ret;
    std::size_t length = a_buf.size();
    const uint8_t* ptrBuf = a_buf.data();


    while (length > 0)
    {
        const uint8_t* onePosition = getFirstOne(ptrBuf, length);
        auto startCodeFound = false;
        if (onePosition == ptrBuf)
        {
            if (mPicture.size() >= 2 && mPicture[mPicture.size() - 2] == 0 && mPicture[mPicture.size() - 1] == 0)
            {
                startCodeFound = true;
            }
        }
        else if (onePosition == ptrBuf + 1)
        {
            if (mPicture.size() >= 1 && mPicture[mPicture.size() - 1] == 0 && *(onePosition - 1) == 0)
            {
                startCodeFound = true;
            }
        }
        else if (onePosition != ptrBuf + length)
        {
            if (*(onePosition - 2) == 0 && *(onePosition - 1) == 0)
            {
                startCodeFound = true;
            }
        }
        const uint8_t* end = (onePosition != ptrBuf + length) ? onePosition + 1 : onePosition;
        std::copy(ptrBuf, end, std::back_inserter(mPicture));
        if (startCodeFound)
        {
            ++m_foundStartCodes;
             std::size_t ind = (onePosition - ptrBuf);
            m_indexes.push_back(ind);
            
            if ((end - ptrBuf) > 4) // check we have more data than just 1 NAL start code
            {
                auto vec = analyze();
                for (auto& l : vec)
                {
                    ret.push_back(l);
                }
            }
            mPicture = { 0, 0, 0, 1 };
        }
        std::size_t diff = (onePosition + 1 - ptrBuf);
        length = diff > length ? 0 : length - diff;
        ptrBuf = onePosition + 1;
    }

    return ret;
}