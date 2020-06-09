/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/WriteSegmentReply>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> WriteSegmentReply::createFrame(int serverId, int offset)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);

    auto frame = CanFrame::create();
    frame->setCanId(0x580 + serverId);

    const uint8_t scs = static_cast<uint8_t>(ServiceReply::Specifier::WriteSegment);
    const uint8_t t   = offset & 1;
    frame->payloadAt(0) = (scs << 5) | (t << 4);

    return frame;
}

String WriteSegmentReply::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "WriteSegmentReply {" << nl
        << "  serverId: " << serverId() << nl
        << "  toggle  : " << toggle() << nl
        << "}";
}

}} // namespace cc::can
