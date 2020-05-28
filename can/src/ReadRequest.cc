/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/ReadRequest>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> ReadRequest::createFrame(int serverId, Selector selector)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);

    auto frame = CanFrame::create();
    frame->setCanId(0x600 + serverId);

    const uint8_t ccs = static_cast<uint8_t>(ServiceRequest::Specifier::Read);
    frame->payloadAt(0) = ccs << 5;
    frame->payloadAt(1) = selector->index();
    frame->payloadAt(2) = selector->index() >> 8;
    frame->payloadAt(3) = selector->subIndex();

    return frame;
}

String ReadRequest::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "ReadRequest {" << nl
        << "  serverId: " << serverId() << nl
        << "  selector: " << selector() << nl
        << "}";
}

}} // namespace cc::can
