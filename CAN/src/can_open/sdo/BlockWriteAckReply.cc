/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/BlockWriteAckReply>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame BlockWriteAckReply::createFrame(int serverId, int sequenceNumber, int newBlockSize)
{
    assert(1 <= serverId && serverId <= 0x7F);
    assert(0 <= sequenceNumber && sequenceNumber <= 0x7F);
    assert(newBlockSize >= 1);

    const uint8_t scs = +Reply::Specifier::BlockWrite;
    const uint8_t ss  = 2;

    return
        CanFrame { 0x580 + static_cast<uint32_t>(serverId) }
        (0, (scs << 5) | ss)
        (1, sequenceNumber)
        (2, newBlockSize);
}

String BlockWriteAckReply::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"sequence-number\": " << sequenceNumber() << ",\n"
        "  \"new-block-size\": " << newBlockSize() << "\n"
        "}";
}

} // namespace cc::can_open::sdo
