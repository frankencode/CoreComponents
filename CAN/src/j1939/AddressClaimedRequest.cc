/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/AddressClaimedRequest>
#include <cc/Format>

namespace cc::j1939 {

CanFrame AddressClaimedRequest::produce(uint8_t dst, uint8_t src)
{
    return
        CanFrame {
            CanId{}
            .prio(6)
            .pf(pf())
            .dst(dst)
            .src(src)
        }
        .size(3)
        (0, 2, 0xEE00); // 0xEE00 == 60928
}

String AddressClaimedRequest::toString() const
{
    return Format{} <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"pgn\": " << pgn() << ",\n"
        "  \"src\": " << CanId{frame_.canId()}.src() << ",\n"
        "  \"dst\": " << CanId{frame_.canId()}.dst() << ",\n"
        "  \"prio\": " << CanId{frame_.canId()}.prio() << "\n"
        "}";
}

} // namespace cc::j1939
