#include <vector>

/// Project files
#include "EsParser.h"

class Mpeg2VideoEsParser : public EsParser
{
public:
    Mpeg2VideoEsParser()
    {
    }
    virtual ~Mpeg2VideoEsParser()
    {
    }

    virtual bool operator()(const uint8_t* from, size_t length);

    std::vector<uint8_t> last;
};
