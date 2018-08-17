#include <vector>
#include <map>

/// Project files
#include "EsParser.h"
#include "GetBits.h"

class Mpeg2VideoEsParser : GetBits, public EsParser
{
    static std::map<uint8_t, std::string> AspectToString;
    static std::map<uint8_t, std::string> FrameRateToString;
public:
    Mpeg2VideoEsParser()
        : foundStartCodes{0}

    {
    }
    virtual ~Mpeg2VideoEsParser()
    {
    }

    virtual bool operator()(const uint8_t* from, ssize_t length);
    bool analyze();

    std::vector<uint8_t> last;
    int foundStartCodes;
    std::vector<uint8_t> mPicture;
};
