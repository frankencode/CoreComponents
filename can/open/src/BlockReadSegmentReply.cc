/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/BlockReadSegmentReply>
#include <cc/Format>

namespace cc {
namespace can {

CanFrame BlockReadSegmentReply::createFrame(int serverId, const String &data, int offset, int sequenceNumber)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);
    CC_ASSERT(data->count() > 0);
    CC_ASSERT(0 <= offset && offset < data->count());
    CC_ASSERT(0 < sequenceNumber && sequenceNumber <= 0x7F);

    CanFrame frame{0x580 + serverId};

    const uint8_t c     = offset + 7 >= data->count();
    const uint8_t seqno = static_cast<const uint8_t>(sequenceNumber);
    frame->payloadAt(0) = (c << 7) | seqno;

    const int n = c ? data->count() - offset : 7;
    for (int i = 0; i < n; ++i)
        frame->payloadAt(1 + i) = data->byteAt(offset + i);

    return frame;
}

String BlockReadSegmentReply::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "BlockReadSegmentReply {" << nl
        << "  serverId      : " << serverId() << nl
        << "  sequenceNumber: " << sequenceNumber() << nl
        << "  isLast        : " << isLast() << nl
        << "  segmentData   : " << hexLine(segmentData()) << nl
        << "}";
}

}} // namespace cc::can
