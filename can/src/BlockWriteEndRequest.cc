/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/BlockWriteEndRequest>
#include <cc/can/Crc16Sink>
#include <cc/Format>
#include <cc/str>

namespace cc {
namespace can {

Ref<CanFrame> BlockWriteEndRequest::createFrame(int serverId, const String &data, bool crcSupport)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);
    CC_ASSERT(data->count() > 0);

    auto frame = CanFrame::create();
    frame->setCanId(0x600 + serverId);

    const uint8_t ccs = static_cast<uint8_t>(ServiceRequest::Specifier::BlockWrite);
    const uint8_t n   = data->count() % 7 == 0 ? 0 : 7 - data->count() % 7;
    const uint8_t cs  = 1;
    const uint16_t crc = crcSupport ? crc16(data) : uint16_t(0);

    frame->payloadAt(0) = (ccs << 5) | (n << 2) | cs;
    frame->setWord(1, 3, crc);

    return frame;
}

String BlockWriteEndRequest::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "BlockWriteEndRequest {" << nl
        << "  serverId            : " << serverId() << nl
        << "  lastSegmentDataCount: " << lastSegmentDataCount() << nl
        << "  crc                 : 0x" << hex(crc(), 4) << nl
        << "}";
}

}} // namespace cc::can
