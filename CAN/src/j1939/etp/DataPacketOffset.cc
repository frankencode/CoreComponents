/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/etp/DataPacketOffset>
#include <cc/j1939/etp/RequestToSend>
#include <cc/Format>

namespace cc::j1939::etp {

CanFrame DataPacketOffset::produce(uint32_t payloadPgn, uint8_t packetCount, uint32_t packetOffset, uint8_t src, uint8_t dst)
{
    return
        CanFrame {
            CanId{}
            .prio(7)
            .pf(pf())
            .dst(dst)
            .src(src)
        }
        (0, +ConnectionManagement::Type::DataPacketOffset)
        (1, packetCount)
        (2, 4, packetOffset)
        (5, 7, payloadPgn);
}

String DataPacketOffset::toString() const
{
    return Format{} <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"pgn\": " << pgn() << ",\n"
        "  \"src\": " << src() << ",\n"
        "  \"dst\": " << dst() << ",\n"
        "  \"payload-pgn\": " << payloadPgn() << ",\n"
        "  \"packet-offset\": " << packetOffset() << ",\n"
        "  \"packet-count\": " << packetCount() << ",\n"
        "  \"prio\": " << prio() << "\n"
        "}";
}

} // namespace cc::j1939::etp
