/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/BlockWriteInitRequest>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame BlockWriteInitRequest::createFrame(int serverId, Selector selector, const Bytes &data)
{
    assert(1 <= serverId && serverId <= 0x7F);
    assert(data.size() > 0);

    const uint8_t ccs = static_cast<uint8_t>(Request::Specifier::BlockWrite);
    const uint8_t cc  = 1; // CRC support
    const uint8_t s   = 1; // total size indicated
    const uint8_t cs  = 0; // sub-command

    return
        CanFrame { 0x600 + static_cast<uint32_t>(serverId) }
        (0, (ccs << 5) | (cc << 2) | (s << 1) | cs)
        (1, selector.index())
        (2, selector.index() >> 8)
        (3, selector.subIndex())
        (4, 7, data.count());
}

String BlockWriteInitRequest::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"selector\": \"" << selector() << "\",\n"
        "  \"total-size\": " << totalSize() << ",\n"
        "  \"crc-support\": " << crcSupport() << "\n"
        "}";
}

} // namespace cc::can_open::sdo
