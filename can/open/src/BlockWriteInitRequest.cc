/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/BlockWriteInitRequest>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> BlockWriteInitRequest::createFrame(int serverId, Selector selector, const String &data)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);
    CC_ASSERT(data->count() > 0);

    auto frame = CanFrame::create();
    frame->setCanId(0x600 + serverId);

    const uint8_t ccs = static_cast<uint8_t>(ServiceRequest::Specifier::BlockWrite);
    const uint8_t cc  = 1; // CRC support
    const uint8_t s   = 1; // total size indicated
    const uint8_t cs  = 0; // sub-command

    frame->payloadAt(0) = (ccs << 5) | (cc << 2) | (s << 1) | cs;
    frame->payloadAt(1) = selector->index();
    frame->payloadAt(2) = selector->index() >> 8;
    frame->payloadAt(3) = selector->subIndex();

    frame->setWord(4, 8, data->count());

    return frame;
}

String BlockWriteInitRequest::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "BlockWriteInitRequest {" << nl
        << "  serverId      : " << serverId() << nl
        << "  selector      : " << selector() << nl
        << "  totalDataCount: " << totalDataCount() << nl
        << "  crcSupport    : " << crcSupport() << nl
        << "}";
}

}} // namespace cc::can
