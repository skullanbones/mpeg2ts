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
     * * @note NOTE! This function does only parse up to 64 bits.
     * Passing a request greater than 64 bits will trigger an out of bound
     * exception.
     * @param requestedBits Number of bits to parse
     * @param data Data to parse
     * @return Parsed bits
     */
    uint64_t getBits(uint8_t requestedBits);

    /*!
     * Resets bit reader to start over reading from a buffer with
     * start index and buffer size as parameters settings.
     * @param srcBytes The buffer to read from
     * @param srcSize The buffer size to read from
     * @param inx The start byte offset to start read from
     */
    void resetBits(const uint8_t* srcBytes, size_t srcSize, size_t inx = 0);

    /*!
     * Skips amount of bits of any size. This function can skip any number
     * of bits as long as its inside the data scope, otherwise it will trigger
     * an out of bound exception.
     * @param skipBits Skip amount of bits.
     */
    void skipBits(uint8_t skipBits);

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

    std::string what()
    {
        return mMsg;
    }

    const std::string mMsg;
};
