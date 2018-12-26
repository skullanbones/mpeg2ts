#include "GetBits.h"

GetBits::GetBits()
    : mNumStoredBits{ 0 }
    , mBitStore{ 0 }
    , mSrcInx{ 0 }
    , mSize{ 0 }
    , mSrcBytes{ nullptr }
{
}

uint64_t GetBits::getBits(uint8_t requestedBits)
{
    uint64_t ret = 0;

    if (mSrcBytes == nullptr)
    {
        throw GetBitsException("null input data");
    }

    if (requestedBits > 64)
    {
        throw GetBitsException("Cannot parse more than 64 individual bits at a time.");
    }

    while (requestedBits > 0u)
    {
        if (mNumStoredBits == 0u)
        {
            if (mSrcInx >= mSize)
            {
                throw GetBitsException("getBits: Out of bound read");
            }

            mNumStoredBits = 8;
            mBitStore = mSrcBytes[mSrcInx++];
        }

        uint8_t bitsToFromStore = mNumStoredBits > requestedBits ? requestedBits : mNumStoredBits;
        ret = (ret << bitsToFromStore) | (mBitStore >> (8 - bitsToFromStore));

        requestedBits -= bitsToFromStore;
        mNumStoredBits -= bitsToFromStore;
        mBitStore = mBitStore << bitsToFromStore;
    }

    return ret;
}

void GetBits::resetBits(const uint8_t* srcBytes, size_t srcSize, size_t inx)
{
    mNumStoredBits = 0;
    mBitStore = 0;
    mSrcInx = inx;
    mSize = srcSize;
    mSrcBytes = srcBytes;
}

void GetBits::skipBits(uint8_t skipBits)
{
    if (skipBits <= 64)
    {
        getBits(skipBits);
        return;
    }

    int n = skipBits / 64;
    int rem = skipBits % 64;

    for (int i = 0; i < n; i++)
    {
        mNumStoredBits = 0;
        mBitStore = 0;
        mSrcInx += 8;

        if (mSrcInx >= mSize)
        {
            throw GetBitsException("skipBits: Out of bound read");
        }
    }

    getBits(rem);
}

void GetBits::skipBytes(uint16_t skipBytes)
{
    if ((mSrcInx + skipBytes) >= mSize)
    {
        throw GetBitsException("getBits: Out of bound read mSrcInx: " + std::to_string(mSrcInx));
    }
    else
    {
        mNumStoredBits = 0;
        mBitStore = 0;
        mSrcInx += skipBytes;
    }
}

size_t GetBits::getByteInx() const
{
    return mNumStoredBits == 0 ? mSrcInx : mSrcInx - 1;
}


void GetBits::printSrcBytes() const
{
    for (size_t i = mSrcInx; i < mSize; i++)
    {
        printf("%02X", mSrcBytes[i]);
    }
    printf("\n");
}
