class BitsGetter
{
public:
    BitsGetter(const uint8_t *srcBytes, size_t srcSize);
    virtual ~BitsGetter();
    /*!
     * Parses maximum 64 bits by bit from data and returns results
     * @param requestedBits Number of bits to parse
     * @param data Data to parse
     * @return Parsed bits
     */
    uint64_t getBits(uint8_t requestedBits, const uint8_t* data);

protected:
    uint8_t mNumStoredBits;
    uint8_t mBitStore;
    size_t mSrcInx;
    size_t mSize;
    uint8_t* srcBytes;
};

struct BitsGetterException
{
    BitsGetterException(const std::string msg);
    virtual ~BitsGetterException();

    const std::string mMsg;
};
