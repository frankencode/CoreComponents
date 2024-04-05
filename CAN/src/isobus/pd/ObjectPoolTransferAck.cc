/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/ObjectPoolTransferAck>
#include <cc/Format>

namespace cc::isobus::pd {

void ObjectPoolTransferAck::printValue(Format &f) const
{
    f << "  \"success\": " << (+errors() == 0) << ",\n";

    if (+errors()) {
        f << "  \"errors\": \"" << errors() << "\",\n";
    }
}

String str(ObjectPoolTransferAck::Errors errors)
{
    List<String> parts;
    if (errors & ObjectPoolTransferAck::Errors::OutOfMemory) parts << "isobus.device.transfer_error.out_of_memory";
    if (errors & ObjectPoolTransferAck::Errors::OtherError) parts << "isobus.device.transfer_error.other";
    return parts.join("|");
}

} // namespace cc::isobus::pd
