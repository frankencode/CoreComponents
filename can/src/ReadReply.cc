/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/ReadReply>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> ReadReply::createFrame(int serverId, Selector selector, const String &data)
{
    CC_ASSERT(1 <= serverId && serverId <= 0x7F);
    CC_ASSERT(data->count() > 0);

    auto frame = CanFrame::create();
    frame->setCanId(0x580 + serverId);

    const uint8_t scs = static_cast<uint8_t>(ServiceReply::Specifier::Read);
    const uint8_t e   = data->count() <= 4;
    const uint8_t n   = (e) ? 4 - data->count() : 0;
    const uint8_t s   = 1;

    frame->payloadAt(0) = (scs << 5) | (n << 2) | (e << 1) | s;
    frame->payloadAt(1) = selector->index();
    frame->payloadAt(2) = selector->index() >> 8;
    frame->payloadAt(3) = selector->subIndex();
    if (e) {
        for (int i = 0; i < data->count(); ++i)
            frame->payloadAt(4 + i) = data->byteAt(i);
    }
    else {
        int h = data->count();
        for (int i = 0; i < 4; ++i) {
            frame->payloadAt(4 + i) = h;
            h >>= 8;
        }
    }

    return frame;
}

String ReadReply::Instance::toString() const
{
    if (!isValid()) return String{};

    Format f;
    f << "ReadReply {" << nl
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
