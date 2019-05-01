#include <memory>


#include "Mpeg2Codec.h"

#include "Mpeg2VideoParser.h"

namespace mpeg2
{

Mpeg2Codec::Mpeg2Codec()
    : mPimpl(new Mpeg2VideoEsParser())
{
}

Mpeg2Codec::~Mpeg2Codec()
{
}


std::vector<EsInfoMpeg2> Mpeg2Codec::parse(const std::vector<uint8_t>& buf)
{
    return mPimpl->parse(buf);
}

/// @brief Analyze the content on data after startcodes.
std::vector<EsInfoMpeg2> Mpeg2Codec::analyze()
{
    return mPimpl->analyze();
}

std::string Mpeg2Codec::toString(Mpeg2Type e)
{
    return mPimpl->toString(e);
}

} // namespace mpeg2