/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/BlockReadStartRequest>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame BlockReadStartRequest::createFrame(int serverId)
{
    assert(1 <= serverId && serverId <= 0x7F);

    const uint8_t ccs = static_cast<uint8_t>(Request::Specifier::BlockRead);
    const uint8_t cs  = 3;

    return
        CanFrame { 0x600 + static_cast<uint32_t>(serverId) }
        (0, (ccs << 5) | cs);
}

String BlockReadStartRequest::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": " << typeName() << ",\n"
        "  \"server\": " << serverId() << "\n"
        "}";
}

} // namespace cc::can_open::sdo
