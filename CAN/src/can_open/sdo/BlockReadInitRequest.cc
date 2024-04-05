/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/BlockReadInitRequest>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame BlockReadInitRequest::createFrame(int serverId, Selector selector, int blockSize, int switchThreshold)
{
    assert(1 <= serverId && serverId <= 0x7F);
    assert(1 <= blockSize && blockSize <= 0x7F);
    assert(0 <= switchThreshold && switchThreshold <= 0xFF);

    const uint8_t ccs = static_cast<const uint8_t>(Request::Specifier::BlockRead);
    const uint8_t cc  = 1;
    const uint8_t cs  = 0;

    return
        CanFrame { 0x600 + static_cast<uint32_t>(serverId) }
        (0, (ccs << 5) | (cc << 2) | cs)
        (1, selector.index())
        (2, selector.index() >> 8)
        (3, selector.subIndex())
        (4, blockSize)
        (5, switchThreshold);
}

String BlockReadInitRequest::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"selector\": \"" << selector() << "\",\n"
        "  \"crc-support\": \"" << crcSupport() << ",\n"
        "  \"block-size\": " << blockSize() << ",\n"
        "  \"switch-threshold\": " << switchThreshold() << "\n"
        "}";
}

} // namespace cc::can_open::sdo
