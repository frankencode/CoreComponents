/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/AbortReply>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame AbortReply::createFrame(int serverId, Selector selector, ErrorCode error)
{
    assert(1 <= serverId && serverId <= 0x7F);

    return
        CanFrame { 0x580 + static_cast<uint32_t>(serverId) }
        (0, +Reply::Specifier::Abort << 5)
        (1, selector.index())
        (2, selector.index() >> 8)
        (3, selector.subIndex())
        (4, 7, +error);
}

String AbortReply::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"selector\": \"" << selector() << "\",\n"
        "  \"error\": \"" << error() << "\"\n"
        "}";
}

String str(const AbortReply &message)
{
    return message.toString();
}

} // namespace cc::can_open::sdo
