/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/BlockWriteInitReply>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame BlockWriteInitReply::createFrame(int serverId, Selector selector, int blockSize)
{
    assert(1 <= serverId && serverId <= 0x7F);
    assert(1 <= blockSize && blockSize <= 0x7F);

    const uint8_t scs = static_cast<uint8_t>(Reply::Specifier::BlockWrite);
    const uint8_t sc  = 1; // CRC support
    const uint8_t ss  = 0; // sub-command

    return
        CanFrame { 0x580 + static_cast<uint32_t>(serverId) }
        (0, (scs << 5) | (sc << 2) | ss)
        (1, selector.index())
        (2, selector.index() >> 8)
        (3, selector.subIndex())
        (4, blockSize);
}

String BlockWriteInitReply::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"selector\": \"" << selector() << "\",\n"
        "  \"block-size\": " << blockSize() << ",\n"
        "  \"crc-support\": " << crcSupport() << "\n"
        "}";
}

} // namespace cc::can_open::sdo
