/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/ReadSegmentRequest>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame ReadSegmentRequest::createFrame(int serverId, bool toggle)
{
    assert(1 <= serverId && serverId <= 0x7F);

    const uint8_t ccs = static_cast<uint8_t>(Request::Specifier::ReadSegment);
    const uint8_t t   = toggle ? 1 : 0;

    return
        CanFrame { 0x600 + static_cast<uint32_t>(serverId) }
        (0, (ccs << 5) | (t << 4));
}

String ReadSegmentRequest::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"can_open.sdo.read_segment_request\",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"toggle\": " << toggle() << "\n"
        "}";
}

} // namespace cc::can_open::sdo
