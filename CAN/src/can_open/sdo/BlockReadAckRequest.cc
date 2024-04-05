/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/BlockReadAckRequest>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame BlockReadAckRequest::createFrame(int serverId, int sequenceNumber, int newBlockSize)
{
    assert(1 <= serverId && serverId <= 0x7F);
    assert(0 <= sequenceNumber && sequenceNumber <= 0x7F);
    assert(1 <= newBlockSize && newBlockSize <= 0x7F);

    const uint8_t ccs = static_cast<const uint8_t>(Request::Specifier::BlockRead);
    const uint8_t cs  = 2;

    return
        CanFrame { 0x600 + static_cast<uint32_t>(serverId) }
        (0, (ccs << 5) | cs)
        (1, sequenceNumber)
        (2, newBlockSize);
}

String BlockReadAckRequest::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": " << typeName() << ",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"sequence-number\": " << sequenceNumber() << ",\n"
        "  \"new-block-size\": " << newBlockSize() << "\n"
        "}";
}

} // namespace cc::can_open::sdo
