/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/BlockReadSegmentReply>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame BlockReadSegmentReply::createFrame(int serverId, const Bytes &data, int offset, int sequenceNumber)
{
    assert(1 <= serverId && serverId <= 0x7F);
    assert(data.count() > 0);
    assert(0 <= offset && offset < data.count());
    assert(0 < sequenceNumber && sequenceNumber <= 0x7F);

    const uint8_t fin   = offset + 7 >= data.count();
    const uint8_t seqno = static_cast<const uint8_t>(sequenceNumber);

    return
        CanFrame { 0x580 + static_cast<uint32_t>(serverId) }
        (0, (fin << 7) | seqno)
        (1, data(offset))
        (2, data(offset + 1))
        (3, data(offset + 2))
        (4, data(offset + 3))
        (5, data(offset + 4))
        (6, data(offset + 5))
        (7, data(offset + 6));
}

String BlockReadSegmentReply::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"sequence-number\": " << sequenceNumber() << ",\n"
        "  \"is-last\": " << isLast() << ",\n"
        "  \"data\": \"" << hexLine(segmentData()) << "\"\n"
        "}";
}

} // namespace cc::can_open::sdo
