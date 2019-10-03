/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/WriteRequest>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> WriteRequest::createFrame(int serverId, Selector selector, const String &data)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);

    auto frame = CanFrame::create();
    frame->setCanId(0x600 + serverId);

    const uint8_t ccs = static_cast<uint8_t>(ServiceRequest::Specifier::Write);
    const uint8_t e   = 0 < data->count() && data->count() <= 4;
    const uint8_t n   = (e) ? (4 - data->count()) : 0;
    const uint8_t s   = data->count() > 0;

    frame->payloadAt(0) = (ccs << 5) | (n << 2) | (e << 1) | s;
    frame->payloadAt(1) = selector->index();
    frame->payloadAt(2) = selector->index() >> 8;
    frame->payloadAt(3) = selector->subIndex();

    if (e) {
        for (int i = 0; i < data->count(); ++i)
            frame->payloadAt(4 + i) = data->byteAt(i);
    }
    else if (s) {
        frame->setWord(4, 8, data->count());
    }

    return frame;
}

String WriteRequest::Instance::toString() const
{
    if (!isValid()) return String{};

    Format f;
    f << "WriteRequest {" << nl
      << "  serverId: " << serverId() << nl
      << "  selector: " << selector() << nl;
    if (hasExpeditedData())
        f << "  data    : " << hexLine(expeditedData()) << nl;
    else if (hasTotalDataCount())
        f << "  length  : " << totalDataCount() << nl;
    f << "}";
    return f;
}

}} // namespace cc::can
