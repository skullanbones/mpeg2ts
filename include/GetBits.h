
#pragma once
#include <cstdint>
#include <string>

class GetBits
{
public:
    GetBits();

    virtual ~GetBits() = default;
    /*!
     * Parses maximum 64 bits by bit from data and returns results
     * @param requestedBits Number of bits to parse
     * @param data Data to parse
     * @return Parsed bits
     */
    uint64_t getBits(uint8_t requestedBits);

    void resetBits(const uint8_t* srcBytes, size_t srcSize, size_t inx = 0);

    /*!
     * Return offset to current byte
     */
    size_t getByteInx();

protected:
    uint8_t mNumStoredBits;
    uint8_t mBitStore;
    size_t mSrcInx;
    size_t mSize;
    const uint8_t* mSrcBytes;
};

struct GetBitsException
{
    GetBitsException(const std::string msg);
    virtual ~GetBitsException() = default;

    const std::string mMsg;
};
