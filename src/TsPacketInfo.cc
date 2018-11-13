#include <public/mpeg2ts.h>

namespace mpeg2ts
{

std::ostream& operator<<(std::ostream& ss, const TsPacketInfo& rhs)
{
    ss << "-------------TsPacketInfo------------- " << std::endl;
    ss << "PID: " << rhs.pid << std::endl;
    ss << "errorIndicator: " << static_cast<int>(rhs.errorIndicator) << std::endl;
    ss << "isPayloadStart: " << static_cast<int>(rhs.isPayloadStart) << std::endl;
    ss << "hasAdaptationField: " << rhs.hasAdaptationField << std::endl;
    ss << "hasPayload: " << rhs.hasPayload << std::endl;
    ss << "hasPrivateData: " << rhs.hasPrivateData << std::endl;
    ss << "hasRandomAccess: " << rhs.hasRandomAccess << std::endl;
    ss << "isScrambled: " << rhs.isScrambled << std::endl;
    ss << "isDiscontinuity: " << rhs.isDiscontinuity << std::endl;

    ss << "pcr: " << rhs.pcr << std::endl;
    ss << "opcr: " << rhs.opcr << std::endl;
    if (rhs.hasPrivateData)
    {
        ss << "privateDataSize: " << rhs.privateDataSize << std::endl;
        ss << "privateDataOffset: " << rhs.privateDataOffset << std::endl;
    }
    if (rhs.hasPayload)
    {
        ss << "payloadSize: " << rhs.payloadSize << std::endl;
        ss << "payloadStartOffset: " << static_cast<int>(rhs.payloadStartOffset) << std::endl;
    }
    ss << "isError: " << rhs.isError << std::endl;
    return ss;
}

} // namespace mpeg2ts
