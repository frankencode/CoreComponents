/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/BlockReadInitReply>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> BlockReadInitReply::createFrame(int serverId, Selector selector, const String &data)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);

    auto frame = CanFrame::create();
    frame->setCanId(0x580 + serverId);

    const uint8_t scs = static_cast<uint8_t>(ServiceReply::Specifier::BlockRead);
    const uint8_t sc  = 1;
    const uint8_t s   = 1;
    const uint8_t ss  = 0;
    frame->payloadAt(0) = (scs << 5) | (sc << 2) | (s << 1) | ss;
    frame->payloadAt(1) = selector->index();
    frame->payloadAt(2) = selector->index() >> 8;
    frame->payloadAt(3) = selector->subIndex();
    frame->setWord(4, 8, data->count());

    return frame;
}

String BlockReadInitReply::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "BlockReadInitReply {" << nl
        << "  serverId      : " << serverId() << nl
        << "  selector      : " << selector() << nl
        << "  crcSupport    : " << crcSupport() << nl
        << "  totalDataCount: " << totalDataCount() << nl
        << "}";
}

}} // namespace cc::can
