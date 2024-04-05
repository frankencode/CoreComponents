/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/tp/RequestToSend>
#include <cc/j1939/DictionaryManager>
#include <cc/Format>

namespace cc::j1939::tp {

CanFrame RequestToSend::produce(uint32_t payloadPgn, uint8_t src, uint8_t dst, const Bytes &message)
{
    assert(message.count() <= 1785);

    return
        CanFrame {
            CanId{}
            .prio(7)
            .pf(pf())
            .dst(dst)
            .src(src)
        }
        (0, +ConnectionManagement::Type::RequestToSend)
        (1, 2, message.count())
        (3, (message.count() + 6) / 7)
        (4, 0xFF)
        (5, 7, payloadPgn);
}

String RequestToSend::toString() const
{
    Format f;
    MessageTypeInfo type;
    if (DictionaryManager{}.messageTypeDictionary().lookup(payloadPgn() << 8, &type)) {
        f <<
            "{\n"
            "  \"type\": \"" << shortName() << "\",\n"
            "  \"pgn\": " << pgn() << ",\n"
            "  \"src\": " << src() << ",\n"
            "  \"dst\": " << dst() << ",\n"
            "  \"payload-pgn\": " << payloadPgn() << ",\n"
            "  \"payload-type\": \"" << type.name() << "\",\n"
            "  \"payload-label\": \"" << type.shortName() << "\",\n"
            "  \"total-size\": " << totalSize() << ",\n"
            "  \"packet-count\": " << packetCount() << ",\n"
            "  \"cleared-packet-count\": " << batchSize() << ",\n"
            "  \"prio\": " << prio() << "\n"
            "}";
    }
    else {
        f <<
            "{\n"
            "  \"type\": \"" << shortName() << "\",\n"
            "  \"pgn\": " << pgn() << ",\n"
            "  \"src\": " << src() << ",\n"
            "  \"dst\": " << dst() << ",\n"
            "  \"payload-pgn\": " << payloadPgn() << ",\n"
            "  \"total-size\": " << totalSize() << ",\n"
            "  \"packet-count\": " << packetCount() << ",\n"
            "  \"cleared-packet-count\": " << batchSize() << ",\n"
            "  \"prio\": " << prio() << "\n"
            "}";
    }
    return f;
}

} // namespace cc::j1939::tp
