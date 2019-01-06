
#pragma once

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
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
    EsParser(std::vector<uint8_t> a_startCode)
        : m_startCode{ a_startCode }
    {
    }

    virtual ~EsParser() = default;


    /// @brief Finds startcode in a binary buffer by using std search algorithm
    /// @param buf The binary data to find startcodes in
    std::vector<std::size_t> findStartCodes(const std::vector<uint8_t>& buf);

protected:
    std::vector<uint8_t> mPicture;
    std::vector<uint8_t> m_startCode;
};

inline std::vector<std::size_t> EsParser::findStartCodes(const std::vector<uint8_t>& a_buf)
{
    std::vector<std::size_t> indexes{};
    auto it{ a_buf.begin() };
    while ((it = std::search(it, a_buf.end(), m_startCode.begin(), m_startCode.end())) != a_buf.end())
    {
        indexes.push_back(std::distance(a_buf.begin(), it++));
    }
    return indexes;
}
