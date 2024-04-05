/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/etp/ClearToSend>
#include <cc/j1939/etp/RequestToSend>
#include <cc/Format>

namespace cc::j1939::etp {

CanFrame ClearToSend::produce(const RequestToSend &request, uint32_t nextPacket)
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
        (1, 0xFF)
        (2, 4, nextPacket)
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

} // namespace cc::j1939::etp
