/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/HeartBeat>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> HeartBeat::createFrame(int sourceId, NodeState nodeState)
{
    CC_ASSERT(1 <= sourceId && sourceId <= 0x7F);
    auto frame = CanFrame::create();
    frame->setCanId(0x700 + sourceId);
    frame->setPayloadCount(1);
    frame->payloadAt(0) = static_cast<uint8_t>(nodeState);
    return frame;
}

String HeartBeat::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "HeartBeat {" << nl
        << "  sourceId : " << sourceId() << nl
        << "  nodeState: " << nodeState() << nl
        << "  toggle   : " << toggle() << nl
        << "}";
}

}} // namespace cc::can
