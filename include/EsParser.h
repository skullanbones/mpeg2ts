#pragma once

#include "Types.h" // For ssize_t

#include <algorithm>
#include <cstdint>
#include <list>
#include <memory>

struct EsInfo
{
    virtual ~EsInfo(){};
};

class EsParser
{
public:
    EsParser()
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

    virtual std::vector<std::shared_ptr<EsInfo>> operator()(const uint8_t* /*from*/, size_t /*length*/)
    {
        return std::vector<std::shared_ptr<EsInfo>>();
    }
};
