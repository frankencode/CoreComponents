/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/etp/EndOfMessage>
#include <cc/j1939/etp/RequestToSend>
#include <cc/Format>

namespace cc::j1939::etp {

CanFrame EndOfMessage::produce(const RequestToSend &request)
{
    return
        CanFrame {
            CanId{}
            .prio(7)
            .pf(pf())
            .dst(request.src())
            .src(request.dst())
        }
        (0, +ConnectionManagement::Type::EndOfMessage)
        (1, 4, request.totalSize())
        (5, 7, request.payloadPgn());
}

String EndOfMessage::toString() const
{
    return Format{} <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"pgn\": " << pgn() << ",\n"
        "  \"src\": " << src() << ",\n"
        "  \"dst\": " << dst() << ",\n"
        "  \"payload-pgn\": " << payloadPgn() << ",\n"
        "  \"total-size\": " << totalSize() << ",\n"
        "  \"prio\": " << prio() << "\n"
        "}";
}

} // namespace cc::j1939::etp
