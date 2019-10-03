/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/WriteReply>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> WriteReply::createFrame(int serverId, Selector selector)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);

    auto frame = CanFrame::create();
    frame->setCanId(0x580 + serverId);

    const uint8_t scs = static_cast<uint8_t>(ServiceReply::Specifier::Write);
    frame->payloadAt(0) = scs << 5;
    frame->payloadAt(1) = selector->index();
    frame->payloadAt(2) = selector->index() >> 8;
    frame->payloadAt(3) = selector->subIndex();
    return frame;
}

String WriteReply::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "WriteReply {" << nl
        << "  serverId: " << serverId() << nl
        << "  selector: " << selector() << nl
        << "}";
}

}} // namespace cc::can
