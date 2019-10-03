/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/AbortReply>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> AbortReply::createFrame(int serverId, Selector selector, Abort::Reason reason)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);

    return Abort::createFrame(0x580 + serverId, static_cast<uint8_t>(ServiceReply::Specifier::Abort), selector, reason);
}

String AbortReply::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "AbortReply {" << nl
        << "  serverId: " << serverId() << nl
        << "  selector: " << selector() << nl
        << "  reason  : " << reason() << nl
        << "}";
}

}} // namespace cc::can
