/*
 * Copyright (C) 2025 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/WebSocketFrame>
#include <cc/WebSocketClose>
#include <cc/WebSocketPing>
#include <cc/WebSocketPong>
#include <cc/bits>

namespace cc {

WebSocketFrame::State::State(ByteSource &source, long maxPayloadSize)
{
    assert(source.endian() == ByteOrder::BigEndian);

    uint8_t h = 0;

    h = source.readUInt8();
    fin_  = bit<7>(h);
    rsv_  = bit<6,4>(h);
    type_ = bit<3,0>(h);

    h = source.readUInt8();
    bool masked = bit<7>(h);

    uint64_t len = bit<6,0>(h);
    if (len == 126) {
        len = source.readUInt16();
    }
    else if (len == 127) {
        len = source.readUInt64();
    }

    if (len > static_cast<uint64_t>(maxPayloadSize)) throw WebSocketFrame::FrameToBig{};

    uint32_t mask = 0;
    if (masked) {
        mask = source.readUInt32();
    }

    payload_ = String::allocate(len);
    source.read(&payload_);

    if (masked) {
        xorPayloadWithMask(mask);
    }
}

void WebSocketFrame::State::writeTo(ByteSink &sink, uint32_t mask)
{
    assert(sink.endian() == ByteOrder::BigEndian);

    uint8_t h = 0;

    bitAssign<7>(h, fin_);
    bitAssign<6,5>(h, rsv_);
    bitAssign<3,0>(h, type_);
    sink.writeUInt8(h);

    bitAssign<7>(h, mask != 0);
    uint8_t len = 0;
    if (payload_.count() <= 125) len = payload_.count();
    else if (payload_.count() <= 65535) len = 126;
    else len = 127;
    bitAssign<6,0>(h, len);
    sink.writeUInt8(h);

    if (len == 126) {
        sink.writeUInt16(static_cast<uint16_t>(payload_.count()));
    }
    else if (len == 127) {
        sink.writeUInt64(static_cast<uint64_t>(payload_.count()));
    }

    if (mask != 0) {
        sink.writeUInt32(mask);
        xorPayloadWithMask(mask);
    }

    sink.write(payload_);
}

void WebSocketFrame::State::xorPayloadWithMask(uint32_t mask)
{
    for (long i = 0, n = payload_.count(); i < n; ++i) {
        unsigned j = i & 0x3;
        payload_[i] ^= static_cast<uint8_t>(mask >> (8 * (3 - j)));
    }
}

} // namespace cc
