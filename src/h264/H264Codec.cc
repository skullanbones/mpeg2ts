#include "H264Codec.h"

#include "H264Parser.h"


namespace h264
{
H264Codec::H264Codec()
    : mPimpl{ new H264EsParser() }
{
}

H264Codec::~H264Codec()
{
}


std::vector<EsInfoH264> H264Codec::parse(const std::vector<uint8_t>& buf)
{
    return mPimpl->parse(buf);
}

std::vector<EsInfoH264> H264Codec::analyze()
{
    return mPimpl->analyze();
}

std::string H264Codec::toString(H264InfoType e)
{
    return mPimpl->toString(e);
}

std::string H264Codec::toString(NalUnitType e)
{
    return mPimpl->toString(e);
}

} // namespace h264