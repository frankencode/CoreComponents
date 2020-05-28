/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/ReadSegmentReply>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> ReadSegmentReply::createFrame(int serverId, const String &data, int fill, int offset)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);
    CC_ASSERT(offset < fill);
    CC_ASSERT(fill <= data->count());

    auto frame = CanFrame::create();
    frame->setCanId(0x580 + serverId);

    const uint8_t scs = static_cast<uint8_t>(ServiceReply::Specifier::ReadSegment);
    const uint8_t t   = offset & 1;
    const uint8_t c   = fill <= offset + 7;
    const uint8_t n   = (c) ? fill - offset : 7;

    frame->payloadAt(0) = (scs << 5) | (t << 4) | ((7 - n) << 1) | c;
    for (int i = 0; i < n; ++i)
        frame->payloadAt(1 + i) = data->byteAt(offset + i);

    return frame;
}

String ReadSegmentReply::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "ReadSegmentReply {" << nl
        << "  serverId: " << serverId() << nl
        << "  toggle  : " << toggle() << nl
        << "  isLast  : " << isLast() << nl
        << "  data    : " << hexLine(data()) << nl
        << "}";
}

}} // namespace cc::can
