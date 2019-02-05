
#include "mpeg2ts.h"

namespace mpeg2ts
{

std::ostream& operator<<(std::ostream& ss, const TsPacketInfo& rhs)
{
    ss << "-------------TsPacketInfo------------- " << '\n';
    ss << "PID: " << rhs.pid << '\n';
    ss << "errorIndicator: " << static_cast<int>(rhs.errorIndicator) << '\n';
    ss << "isPayloadStart: " << static_cast<int>(rhs.isPayloadStart) << '\n';
    ss << "hasAdaptationField: " << rhs.hasAdaptationField << '\n';
    ss << "hasPayload: " << rhs.hasPayload << '\n';
    ss << "hasPrivateData: " << rhs.hasPrivateData << '\n';
    ss << "hasRandomAccess: " << rhs.hasRandomAccess << '\n';
    ss << "isScrambled: " << rhs.isScrambled << '\n';
    ss << "isDiscontinuity: " << rhs.isDiscontinuity << '\n';

    ss << "pcr: " << rhs.pcr << '\n';
    ss << "opcr: " << rhs.opcr << '\n';
    if (rhs.hasPrivateData)
    {
        ss << "privateDataSize: " << rhs.privateDataSize << '\n';
        ss << "privateDataOffset: " << rhs.privateDataOffset << '\n';
    }
    if (rhs.hasPayload)
    {
        ss << "payloadSize: " << rhs.payloadSize << '\n';
        ss << "payloadStartOffset: " << static_cast<int>(rhs.payloadStartOffset) << '\n';
    }
    ss << "isError: " << rhs.isError << '\n';
    return ss;
}

} // namespace mpeg2ts
