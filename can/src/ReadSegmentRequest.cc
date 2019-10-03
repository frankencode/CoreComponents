/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/ReadSegmentRequest>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> ReadSegmentRequest::createFrame(int serverId, bool toggle)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);

    auto frame = CanFrame::create();
    frame->setCanId(0x600 + serverId);

    const uint8_t ccs = static_cast<uint8_t>(ServiceRequest::Specifier::ReadSegment);
    const uint8_t t   = toggle ? 1 : 0;
    frame->payloadAt(0) = (ccs << 5) | (t << 4);

    return frame;
}

String ReadSegmentRequest::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "ReadSegmentRequest {" << nl
        << "  serverId: " << serverId() << nl
        << "  toggle  : " << toggle() << nl
        << "}";
}

}} // namespace cc::can
