/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/etp/Abort>
#include <cc/j1939/etp/RequestToSend>
#include <cc/Format>

namespace cc::j1939::etp {

CanFrame Abort::produce(const RequestToSend &request, Reason reason)
{
    return
        CanFrame {
            CanId{}
            .prio(7)
            .pf(pf())
            .dst(request.src())
            .src(request.dst())
        }
        (0, +ConnectionManagement::Type::Abort)
        (1, +reason)
        (2, 0xFF)
        (3, 0xFF)
        (4, 0xFF)
        (5, 7, request.payloadPgn());
}

String Abort::toString() const
{
    return Format{} <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"pgn\": " << pgn() << ",\n"
        "  \"src\": " << src() << ",\n"
        "  \"dst\": " << dst() << ",\n"
        "  \"payload-pgn\": " << payloadPgn() << ",\n"
        "  \"prio\": " << prio() << "\n"
        "}";
}

} // namespace cc::j1939::etp
