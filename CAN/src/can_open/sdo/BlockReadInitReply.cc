/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/BlockReadInitReply>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame BlockReadInitReply::createFrame(int serverId, Selector selector, const Bytes &data)
{
    assert(1 <= serverId && serverId <= 0x7F);

    const uint8_t scs = static_cast<uint8_t>(Reply::Specifier::BlockRead);
    const uint8_t sc  = 1;
    const uint8_t s   = 1;
    const uint8_t ss  = 0;

    return
        CanFrame { 0x580 + static_cast<uint32_t>(serverId) }
        (0, (scs << 5) | (sc << 2) | (s << 1) | ss)
        (1, selector.index())
        (2, selector.index() >> 8)
        (3, selector.subIndex())
        (4, 7, data.count());
}

String BlockReadInitReply::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"selector\": \"" << selector() << "\",\n"
        "  \"crc-support\": " << crcSupport() << ",\n"
        "  \"total-size\": " << totalSize() << "\n"
        "}";
}

} // namespace cc::can_open::sdo
