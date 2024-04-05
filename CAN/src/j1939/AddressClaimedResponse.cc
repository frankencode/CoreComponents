/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/AddressClaimedResponse>
#include <cc/j1939/FunctionId>
#include <cc/Format>

namespace cc::j1939 {

CanFrame AddressClaimedResponse::produce(uint8_t src, uint64_t name)
{
    return
        CanFrame {
            CanId{}
            .prio(6)
            .pf(pf())
            .dst(0xFF)
            .src(src)
        }
        .size(8)
        (0, 7, name);
}

uint64_t AddressClaimedResponse::functionId() const
{
    return frame_.data<uint64_t>();
}

String AddressClaimedResponse::toString() const
{
    uint64_t data = frame_.data<uint64_t>();
    String hexData = hex(data);
    uint8_t src = CanId{frame_.canId()}.src();

    return Format{} <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"pgn\": " << pgn() << ",\n"
        "  \"src\": " << src << ",\n"
        "  \"function\": \"0x" << hexData << "\",\n"
        "  \"details\": " << FunctionId{data}.toString().replaced("\n", "\n  ") << ",\n"
        "  \"prio\": " << CanId{frame_.canId()}.prio() << "\n"
        "}";
}

} // namespace cc::j1939
