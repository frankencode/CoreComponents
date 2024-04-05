/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/HeartBeat>
#include <cc/Format>
#include <cassert>

namespace cc::can_open {

CanFrame HeartBeat::createFrame(int sourceId, NodeState nodeState)
{
    assert(1 <= sourceId && sourceId <= 0x7F);

    return
        CanFrame { 0x700 + static_cast<uint32_t>(sourceId) }
        .size(1)
        (0, +nodeState);
}

String HeartBeat::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"source\": " << sourceId() << ",\n"
        "  \"state\": \"" << nodeState() << "\",\n"
        "  \"toggle\": " << toggle() << "\n"
        "}";
}

String str(const HeartBeat &message)
{
    return message.toString();
}

} // namespace cc::can_open
