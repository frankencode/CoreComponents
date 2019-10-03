/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/BlockWriteEndReply>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> BlockWriteEndReply::createFrame(int serverId)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);

    auto frame = CanFrame::create();
    frame->setCanId(0x580 + serverId);

    const uint8_t scs = static_cast<const uint8_t>(ServiceReply::Specifier::BlockWrite);
    const uint8_t ss  = 1;

    frame->payloadAt(0) = (scs << 5) | ss;
    return frame;
}

String BlockWriteEndReply::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "BlockWriteEndReply{" << nl
        << "  serverId: " << serverId() << nl
        << "}";
}

}} // namespace cc::can
