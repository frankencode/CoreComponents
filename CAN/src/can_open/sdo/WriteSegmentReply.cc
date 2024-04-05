/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/WriteSegmentReply>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame WriteSegmentReply::createFrame(int serverId, int offset)
{
    assert(1 <= serverId && serverId <= 0x7F);

    const uint8_t scs = static_cast<uint8_t>(Reply::Specifier::WriteSegment);
    const uint8_t t   = offset & 1;

    return
        CanFrame { 0x580 + static_cast<uint32_t>(serverId) }
        (0, (scs << 5) | (t << 4));
}

String WriteSegmentReply::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"toggle\": " << toggle() << "\n"
        "}";
}

} // namespace cc::can_open::sdo
