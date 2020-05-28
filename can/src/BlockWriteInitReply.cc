/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/BlockWriteInitReply>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> BlockWriteInitReply::createFrame(int serverId, Selector selector, int blockSize)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);
    CC_ASSERT(1 <= blockSize && blockSize <= 0x7F);

    auto frame = CanFrame::create();
    frame->setCanId(0x580 + serverId);

    const uint8_t scs = static_cast<uint8_t>(ServiceReply::Specifier::BlockWrite);
    const uint8_t sc  = 1; // CRC support
    const uint8_t ss  = 0; // sub-command

    frame->payloadAt(0) = (scs << 5) | (sc << 2) | ss;
    frame->payloadAt(1) = selector->index();
    frame->payloadAt(2) = selector->index() >> 8;
    frame->payloadAt(3) = selector->subIndex();
    frame->payloadAt(4) = blockSize;

    return frame;
}

String BlockWriteInitReply::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "BlockWriteInitReply {" << nl
        << "  serverId  : " << serverId() << nl
        << "  selector  : " << selector() << nl
        << "  blockSize : " << blockSize() << nl
        << "  crcSupport: " << crcSupport() << nl
        << "}";
}

}} // namespace cc::can
