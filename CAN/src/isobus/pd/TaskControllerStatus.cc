/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/TaskControllerStatus>
#include <cc/j1939/AddressClaimManager>
#include <cc/Format>

namespace cc::isobus::pd {

String TaskControllerStatus::toString() const
{
    Format f;

    f <<
        "{\n"
        "  \"type\": \"" << isobus::pd::Message::shortName() << "\",\n"
        // "  \"pd.type-id\": " << static_cast<uint8_t>(isobus::pd::Message::type()) << ",\n"
        "  \"pgn\": " << j1939::Message::pgn() << ",\n"
        "  \"src\": " << j1939::CanId{frame_}.src() << ",\n"
        "  \"dst\": " << j1939::CanId{frame_}.dst() << ",\n";

    String srcName = j1939::AddressClaimManager{}.name(src());
    String dstName = j1939::AddressClaimManager{}.name(dst());

    if (srcName != "") {
        f << "  \"src-name\": \"" << srcName << "\",\n";
    }
    if (dstName != "") {
        f << "  \"dst-name\": \"" << dstName << "\",\n";
    }

    String statusDetails;
    {
        List<String> parts;
        if (status() & +Mask::Active) parts << "isobus.pd.task_controller_status.task_active";
        else parts << "isobus.pd.task_controller_status.task_inactive";
        if (status() & +Mask::BusyWritingToDisk) parts << "isobus.pd.task_controller_status.busy_disk_write";
        if (status() & +Mask::BusyReadingFromDisk) parts << "isobus.pd.task_controller_status.busy_disk_read";
        if (status() & +Mask::BusyExecutingCommand) parts << "isobus.pd.task_controller_status.busy_command_exec";
        if (status() & +Mask::OutOfMemory) parts << "isobus.pd.task_controller_status.out_of_memory";
        statusDetails = parts.join('|');
    }

    f <<
        "  \"status\": \"" << statusDetails << "\",\n"
        "  \"prio\": " << j1939::CanId{frame_}.prio() << "\n"
        "}";

    return f;
}

} // namespace cc::isobus::pd
