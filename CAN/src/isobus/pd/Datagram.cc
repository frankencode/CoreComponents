/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/Datagram>

namespace cc::isobus::pd {

const char *Datagram::shortName() const
{
    const char *typeNames[16] = {
        "isobus.pd.query_capabilities",
        "isobus.pd.manage_file_descriptor",
        "isobus.pd.value_request",
        "isobus.pd.value_update",
        "isobus.pd.value_request_per_time_interval",
        "isobus.pd.value request per_travel_distance",
        "isobus.pd.value request_when_above_threshold",
        "isobus.pd.value_request_when_below_threshold",
        "isobus.pd.value_request_when_changed_by_threshold",
        "isobus.pd.assign_peer_control",
        "isobus.pd.set_value",
        "isobus.pd.reserved_1",
        "isobus.pd.reserved_2",
        "isobus.pd.set_value_ack",
        "isobus.pd.task_controller_status",
        "isobus.pd.client_task_status"
    };

    return typeNames[static_cast<uint8_t>(type()) & 0xF];
}

} // namespace cc::isobus::pd
