/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/BlockReadEndReply>
#include <cc/can_open/Crc16Sink>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame BlockReadEndReply::createFrame(int serverId, const Bytes &data, bool crcSupport)
{
    assert(1 <= serverId && serverId <= 0x7F);
    assert(data.count() > 0);

    const uint8_t scs = +Reply::Specifier::BlockRead;
    const uint8_t ss  = 1;
    const uint8_t n   = (data.count() % 7 == 0) ? 0 : 7 - data.count() % 7;

    return
        CanFrame { 0x580 + static_cast<uint32_t>(serverId) }
        (0, (scs << 5) | (n << 2) | ss)
        (1, 2, crcSupport ? crc16(data) : UINT16_C(0));
}

String BlockReadEndReply::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"crc\": 0x" << hex(crc(), 4) << ",\n"
        "  \"final-segment-size\": " << finalSegmentSize() << "\n"
        "}";
}

} // namespace cc::can_open::sdo
