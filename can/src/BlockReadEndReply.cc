/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/BlockReadEndReply>
#include <cc/can/Crc16Sink>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> BlockReadEndReply::createFrame(int serverId, const String &data, bool crcSupport)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);
    CC_ASSERT(data->count() > 0);

    auto frame = CanFrame::create();
    frame->setCanId(0x580 + serverId);

    const uint8_t scs = static_cast<const uint8_t>(ServiceReply::Specifier::BlockRead);
    const uint8_t ss  = 1;
    const uint8_t n   = (data->count() % 7 == 0) ? 0 : 7 - data->count() % 7;
    frame->payloadAt(0) = (scs << 5) | (n << 2) | ss;

    const uint16_t crc = crcSupport ? crc16(data) : 0U;
    frame->setWord(1, 3, crc);

    return frame;
}

String BlockReadEndReply::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "BlockReadEndReply {" << nl
        << "  serverId            : " << serverId() << nl
        << "  crc                 : 0x" << hex(crc(), 4) << nl
        << "  lastSegmentDataCount: " << lastSegmentDataCount() << nl
        << "}";
}

}} // namespace cc::can
