/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/WriteSegmentRequest>
#include <cc/Format>

namespace cc::can_open::sdo {

CanFrame WriteSegmentRequest::createFrame(int serverId, const Bytes &data, int offset)
{
    assert(1 <= serverId && serverId <= 0x7F);
    assert(offset < data.count());

    const uint8_t ccs = static_cast<uint8_t>(Request::Specifier::WriteSegment);
    const uint8_t t   = offset & 1;
    const uint8_t c   = data.count() <= offset + 7;
    const uint8_t n   = (c) ? data.count() - offset : 7;

    CanFrame frame{0x600 + static_cast<uint32_t>(serverId)};

    frame(0, (ccs << 5) | (t << 4) | ((7 - n) << 1) | c);
    for (int i = 0; i < n; ++i) {
        frame(1 + i, data(offset + i));
    }

    return frame;
}

String WriteSegmentRequest::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"is-last\": " << isLast() << ",\n"
        "  \"data\": " << hexLine(data()) << ",\n"
        "  \"toggle\": " << toggle() << "\n"
        "}";
}

} // namespace cc::can_open::sdo
