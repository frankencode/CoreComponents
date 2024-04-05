/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/DeviceDescription>

namespace cc::isobus::pd {

const char *DeviceDescription::shortName(MessageType type)
{
    const char *typeNames[16] = {
        "isobus.device.structure_label_request",
        "isobus.device.structure_label_reply",
        "isobus.device.localization_label_request",
        "isobus.device.localization_label_reply",
        "isobus.device.object_pool_transfer_request",
        "isobus.device.object_pool_transfer_reply",
        "isobus.device.object_pool_transfer",
        "isobus.device.object_pool_transfer_ack",
        "isobus.device.object_pool_activation_request",
        "isobus.device.object_pool_activation_reply",
        "isobus.device.object_pool_deletion_request",
        "isobus.device.object_pool_deletion_reply",
        "isobus.device.designator_change_request",
        "isobus.device.designator_change_reply",
        "isobus.device.reserved_14",
        "isobus.device.reserved_15"
    };

    return typeNames[static_cast<uint8_t>(type)];
}

} // namespace cc::isobus::pd
