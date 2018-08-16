#include <vector>

/// Project files
#include "EsParser.h"

class Mpeg2VideoEsParser : public EsParser
{
public:
    Mpeg2VideoEsParser()
        : foundStartCodes{0}
    {
    }
    virtual ~Mpeg2VideoEsParser()
    {
    }

    virtual bool operator()(const uint8_t* from, ssize_t length);

    std::vector<uint8_t> last;
    int foundStartCodes;
};
