/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/BlockWriteEndRequest>
#include <cc/can_open/Crc16Sink>
#include <cc/Format>
#include <cc/str>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame BlockWriteEndRequest::createFrame(int serverId, const Bytes &data, bool crcSupport)
{
    assert(1 <= serverId && serverId <= 0x7F);
    assert(data.count() > 0);


    const uint8_t ccs  = +Request::Specifier::BlockWrite;
    const uint8_t n    = data.count() % 7 == 0 ? 0 : 7 - data.count() % 7;
    const uint8_t cs   = 1;
    const uint16_t crc = crcSupport ? crc16(data) : uint16_t(0);

    return
        CanFrame { 0x600 + static_cast<uint32_t>(serverId) }
        (0, (ccs << 5) | (n << 2) | cs)
        (1, 2, crc);
}

String BlockWriteEndRequest::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"last-segment-data-count\": " << lastSegmentDataCount() << ",\n"
        "  \"crc\": 0x" << hex(crc(), 4) << "\n"
        "}";
}

} // namespace cc::can_open::sdo
