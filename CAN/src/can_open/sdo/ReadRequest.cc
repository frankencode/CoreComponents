/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/ReadRequest>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame ReadRequest::createFrame(int serverId, Selector selector)
{
    assert(1 <= serverId && serverId <= 0x7F);

    const uint8_t ccs = static_cast<uint8_t>(Request::Specifier::Read);

    return
        CanFrame { 0x600 + static_cast<uint32_t>(serverId) }
        (0, ccs << 5)
        (1, selector.index())
        (2, selector.index() >> 8)
        (3, selector.subIndex());
}

String ReadRequest::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": " << typeName() << ",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"selector\": \"" << selector() << "\"\n"
        "}";
}

} // namespace cc::can_open::sdo
