/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/ReadSegmentReply>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame ReadSegmentReply::createFrame(int serverId, const Bytes &data, int fill, int offset)
{
    assert(1 <= serverId && serverId <= 0x7F);
    assert(offset < fill);
    assert(fill <= data.count());

    const uint8_t scs = static_cast<uint8_t>(Reply::Specifier::ReadSegment);
    const uint8_t t   = offset & 1;
    const uint8_t c   = fill <= offset + 7;
    const uint8_t n   = (c) ? fill - offset : 7;

    return
        CanFrame { 0x580 + static_cast<uint32_t>(serverId) }
        (0, (scs << 5) | (t << 4) | ((7 - n) << 1) | c)
        (1, data(offset))
        (2, data(offset + 1))
        (3, data(offset + 2))
        (4, data(offset + 3))
        (5, data(offset + 4))
        (6, data(offset + 5))
        (7, data(offset + 6));
}

String ReadSegmentReply::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << "\",\n"
        "  \"toggle\": " << toggle() << ",\n"
        "  \"is-last\": " << isLast() << ",\n"
        "  \"data\": \"" << hexLine(data()) << "\"\n"
        "}";
}

} // namespace cc::can_open::sdo
