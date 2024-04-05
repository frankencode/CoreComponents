/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/tp/ClearToSend>
#include <cc/j1939/tp/RequestToSend>
#include <cc/Format>

namespace cc::j1939::tp {

CanFrame ClearToSend::produce(const RequestToSend &request, uint8_t nextPacket)
{
    return
        CanFrame {
            CanId{}
            .prio(7)
            .pf(pf())
            .dst(request.src())
            .src(request.dst())
        }
        (0, +ConnectionManagement::Type::ClearToSend)
        (1, request.batchSize() < request.packetCount() ? request.batchSize() : request.packetCount())
        (2, nextPacket)
        (3, 0xFF)
        (4, 0xFF)
        (5, 7, request.payloadPgn());
}

String ClearToSend::toString() const
{
    return Format{} <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"pgn\": " << pgn() << ",\n"
        "  \"src\": " << src() << ",\n"
        "  \"dst\": " << dst() << ",\n"
        "  \"payload-pgn\": " << payloadPgn() << ",\n"
        "  \"cleared-packet-count\": " << clearedPacketCount() << ",\n"
        "  \"next-packet-number\": " << nextPacketNumber() << ",\n"
        "  \"prio\": " << prio() << "\n"
        "}";
}

} // namespace cc::j1939::tp
