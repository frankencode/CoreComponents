/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/tp/DataPacket>
#include <cc/HexDump>
#include <cc/Format>

namespace cc::j1939::tp {

CanFrame DataPacket::produce(const Bytes &message, uint8_t packetNumber, uint8_t src, uint8_t dst)
{
    const uint16_t offset = (packetNumber - 1) * 7;

    return
        CanFrame {
            CanId{}
            .prio(7)
            .pf(pf())
            .dst(dst)
            .src(src)
        }
        (0, packetNumber)
        (1, message.has(offset + 0) ? message(offset + 0) : 0xFF)
        (2, message.has(offset + 1) ? message(offset + 1) : 0xFF)
        (3, message.has(offset + 2) ? message(offset + 2) : 0xFF)
        (4, message.has(offset + 3) ? message(offset + 3) : 0xFF)
        (5, message.has(offset + 4) ? message(offset + 4) : 0xFF)
        (6, message.has(offset + 5) ? message(offset + 5) : 0xFF)
        (7, message.has(offset + 6) ? message(offset + 6) : 0xFF);
}

String DataPacket::toString() const
{
    return Format{} <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"pgn\": " << pgn() << ",\n"
        "  \"src\": " << src() << ",\n"
        "  \"dst\": " << dst() << ",\n"
        "  \"seq\": " << packetNumber() << ",\n"
        "  \"prio\": " << prio() << "\n"
        "}";
}

} // namespace cc::j1939::tp
