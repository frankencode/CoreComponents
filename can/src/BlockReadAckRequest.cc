/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/BlockReadAckRequest>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> BlockReadAckRequest::createFrame(int serverId, int sequenceNumber, int newBlockSize)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);
    CC_ASSERT(0 <= sequenceNumber && sequenceNumber <= 0x7F);
    CC_ASSERT(1 <= newBlockSize && newBlockSize <= 0x7F);

    auto frame = CanFrame::create();
    frame->setCanId(0x600 + serverId);

    const uint8_t ccs = static_cast<const uint8_t>(ServiceRequest::Specifier::BlockRead);
    const uint8_t cs  = 2;
    frame->payloadAt(0) = (ccs << 5) | cs;
    frame->payloadAt(1) = sequenceNumber;
    frame->payloadAt(2) = newBlockSize;

    return frame;
}

String BlockReadAckRequest::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "BlockReadAckRequest {" << nl
        << "  serverId      : " << serverId() << nl
        << "  sequenceNumber: " << sequenceNumber() << nl
        << "  newBlockSize  : " << newBlockSize() << nl
        << "}";
}

}} // namespace cc::can
