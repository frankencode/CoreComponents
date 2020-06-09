/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/BlockWriteAckReply>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> BlockWriteAckReply::createFrame(int serverId, int sequenceNumber, int newBlockSize)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);
    CC_ASSERT(0 <= sequenceNumber && sequenceNumber <= 0x7F);
    CC_ASSERT(newBlockSize >= 1);

    auto frame = CanFrame::create();
    frame->setCanId(0x580 + serverId);

    const uint8_t scs = static_cast<uint8_t>(ServiceReply::Specifier::BlockWrite);
    const uint8_t ss  = 2;
    frame->payloadAt(0) = (scs << 5) | ss;
    frame->payloadAt(1) = sequenceNumber;
    frame->payloadAt(2) = newBlockSize;

    return frame;
}

String BlockWriteAckReply::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "BlockWriteAckReply {" << nl
        << "  serverId      : " << serverId() << nl
        << "  sequenceNumber: " << sequenceNumber() << nl
        << "  newBlockSize  : " << newBlockSize() << nl
        << "}";
}

}} // namespace cc::can
