/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/VersionRequest>
#include <cc/j1939/AddressClaimManager>
#include <cc/Format>

namespace cc::isobus::pd {

String VersionRequest::toString() const
{
    Format f;
    f <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"pgn\": " << pgn() << ",\n"
        "  \"src\": " << src() << ",\n"
        "  \"dst\": " << dst() << ",\n";

    String srcName = j1939::AddressClaimManager{}.name(src());
    String dstName = j1939::AddressClaimManager{}.name(dst());

    if (srcName != "") {
        f << "  \"src-name\": \"" << srcName << "\",\n";
    }
    if (dstName != "") {
        f << "  \"dst-name\": \"" << dstName << "\",\n";
    }

    f <<
        "  \"prio\": " << j1939::CanId{frame_}.prio() << "\n"
        "}";

    return f;
}

} // namespace cc::isobus::pd
