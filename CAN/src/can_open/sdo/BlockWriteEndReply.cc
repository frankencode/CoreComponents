/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/BlockWriteEndReply>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame BlockWriteEndReply::createFrame(int serverId)
{
    assert(1 <= serverId && serverId <= 0x7F);

    const uint8_t scs = +Reply::Specifier::BlockWrite;
    const uint8_t ss  = 1;

    return
        CanFrame { 0x580 + static_cast<uint32_t>(serverId) }
        (0, (scs << 5) | ss);
}

String BlockWriteEndReply::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << "\n"
        "}";
}

} // namespace cc::can_open::sdo
