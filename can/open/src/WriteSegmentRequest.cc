/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/WriteSegmentRequest>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> WriteSegmentRequest::createFrame(int serverId, const String &data, int offset)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);
    CC_ASSERT(offset < data->count());

    auto frame = CanFrame::create();
    frame->setCanId(0x600 + serverId);

    const uint8_t ccs = static_cast<uint8_t>(ServiceRequest::Specifier::WriteSegment);
    const uint8_t t   = offset & 1;
    const uint8_t c   = data->count() <= offset + 7;
    const uint8_t n   = (c) ? data->count() - offset : 7;

    frame->payloadAt(0) = (ccs << 5) | (t << 4) | ((7 - n) << 1) | c;
    for (int i = 0; i < n; ++i)
        frame->payloadAt(1 + i) = data->byteAt(offset + i);

    return frame;
}

String WriteSegmentRequest::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "WriteSegmentRequest {" << nl
        << "  serverId: " << serverId() << nl
        << "  isLast  : " << isLast() << nl
        << "  data    : " << hexLine(data()) << nl
        << "  toggle  : " << toggle() << nl
        << "}";
}

}} // namespace cc::can
