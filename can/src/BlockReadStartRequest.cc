/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/BlockReadStartRequest>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> BlockReadStartRequest::createFrame(int serverId)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);

    auto frame = CanFrame::create();
    frame->setCanId(0x600 + serverId);

    const uint8_t ccs = static_cast<uint8_t>(ServiceRequest::Specifier::BlockRead);
    const uint8_t cs  = 3;
    frame->payloadAt(0) = (ccs << 5) | cs;

    return frame;
}

String BlockReadStartRequest::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "BlockReadStartRequest {" << nl
        << "  serverId: " << serverId() << nl
        << "}";
}

}} // namespace cc::can
