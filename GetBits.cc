#include "GetBits.h"

GetBits::GetBits()
    : mNumStoredBits{0}
    , mBitStore{0}
    , mSrcInx{0}
    , mSize{0}
    ,mSrcBytes{0}
{

}

GetBits::~GetBits()
{

}

uint64_t GetBits::getBits(uint8_t requestedBits)
{
    uint64_t ret = 0;

    if (mSrcBytes == nullptr)
    {
        throw GetBitsException("null input data");
    }
    
    while(requestedBits)
    {
        uint8_t bitsToFromStore = mNumStoredBits > requestedBits ? requestedBits : mNumStoredBits;
        ret = (ret << bitsToFromStore) | (mBitStore >> (8 - bitsToFromStore));

        requestedBits -= bitsToFromStore;
        mNumStoredBits -= bitsToFromStore;
        mBitStore <<= bitsToFromStore;

        if (mNumStoredBits == 0)
        {
            if (mSrcInx >= mSize)
            {
                throw GetBitsException("Out of bound read");
            }

            mNumStoredBits = 8;
            mBitStore = mSrcBytes[mSrcInx++];
        }
    }

    return ret;
};

void GetBits::resetBits(const uint8_t *srcBytes, size_t srcSize)
{
    mNumStoredBits = 0;
    mBitStore = 0;
    mSrcInx = 0;
    mSize = srcSize;
    mSrcBytes = srcBytes;
}

GetBitsException::GetBitsException(const std::string msg)
    : mMsg{msg}
{

}

GetBitsException::~GetBitsException()
{

}

