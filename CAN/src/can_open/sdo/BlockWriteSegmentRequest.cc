/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/BlockWriteSegmentRequest>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame BlockWriteSegmentRequest::createFrame(int serverId, const Bytes &data, int offset, int sequenceNumber)
{
    assert(1 <= serverId && serverId <= 0x7F);
    assert(data.count() > 0);
    assert(0 <= offset && offset < data.count());
    assert(1 <= sequenceNumber && sequenceNumber <= 0x7F);

    const uint8_t c     = offset + 7 >= data.count();
    const uint8_t seqno = sequenceNumber;

    CanFrame frame { 0x600 + static_cast<uint32_t>(serverId) };

    frame(0, (c << 7) | seqno);

    const int n = (c) ? data.count() - offset : 7;
    for (int i = 0; i < n; ++i) {
        frame(1 + i, data(offset + i));
    }

    return frame;
}

String BlockWriteSegmentRequest::toString() const
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
