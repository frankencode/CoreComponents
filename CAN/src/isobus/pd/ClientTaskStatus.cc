/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/ClientTaskStatus>
#include <cc/j1939/AddressClaimManager>
#include <cc/Format>

namespace cc::isobus::pd {

String ClientTaskStatus::toString() const
{
    Format f;

    f <<
        "{\n"
        "  \"type\": \"" << isobus::pd::Message::shortName() << "\",\n"
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

    const char *statusDetails =
        (status() & +Mask::Active) ?
        "isobus.pd.client_task_status.task_active" :
        "isobus.pd.client_task_status.task_inactive";

    f <<
        "  \"status\": \"" << statusDetails << "\",\n"
        "  \"prio\": " << prio() << "\n"
        "}";

    return f;
}

} // namespace cc::isobus::pd
