/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/tp/BroadcastAnnounce>
#include <cc/Format>

namespace cc::j1939::tp {

CanFrame BroadcastAnnounce::produce(uint32_t payloadPgn, const Bytes &message, uint8_t src)
{
    assert(message.count() <= 1785);

    return
        CanFrame {
            CanId{}
            .prio(7)
            .pf(pf())
            .dst(0xFF)
            .src(src)
        }
        (0, +ConnectionManagement::Type::BroadcastAnnounce)
        (1, 2, message.count())
        (3, (message.count() + 6 ) / 7)
        (4, 0xFF)
        (5, 7, payloadPgn);
}

String BroadcastAnnounce::toString() const
{
    return Format{} <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"pgn\": " << pgn() << ",\n"
        "  \"src\": " << src() << ",\n"
        "  \"dst\": " << dst() << ",\n"
        "  \"payload-pgn\": " << payloadPgn() << ",\n"
        "  \"total-size\": " << totalSize() << ",\n"
        "  \"packet-count\": " << packetCount() << ",\n"
        "  \"prio\": " << prio() << "\n"
        "}";
}

} // namespace cc::j1939::tp
