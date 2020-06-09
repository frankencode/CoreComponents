/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/BlockReadInitRequest>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> BlockReadInitRequest::createFrame(int serverId, Selector selector, int blockSize, int switchThreshold)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);
    CC_ASSERT(1 <= blockSize && blockSize <= 0x7F);
    CC_ASSERT(0 <= switchThreshold && switchThreshold <= 0xFF);

    auto frame = CanFrame::create();
    frame->setCanId(0x600 + serverId);

    const uint8_t ccs = static_cast<const uint8_t>(ServiceRequest::Specifier::BlockRead);
    const uint8_t cc  = 1;
    const uint8_t cs  = 0;
    frame->payloadAt(0) = (ccs << 5) | (cc << 2) | cs;
    frame->payloadAt(1) = selector->index();
    frame->payloadAt(2) = selector->index() >> 8;
    frame->payloadAt(3) = selector->subIndex();
    frame->payloadAt(4) = blockSize;
    frame->payloadAt(5) = switchThreshold;

    return frame;
}

String BlockReadInitRequest::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "BlockReadInitRequest {" << nl
        << "  serverId       : " << serverId() << nl
        << "  selector       : " << selector() << nl
        << "  crcSupport     : " << crcSupport() << nl
        << "  blockSize      : " << blockSize() << nl
        << "  switchThreshold: " << switchThreshold() << nl
        << "}";
}

}} // namespace cc::can
