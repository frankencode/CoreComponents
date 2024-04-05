/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/DeviceObject>
#include <cc/Format>

namespace cc::isobus::pd {

String DeviceObject::toString() const
{
    return Format{} <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"designator\": \"" << designator() << "\",\n"
        "  \"software-version\": \"" << softwareVersion() << "\",\n"
        "  \"client\": " << functionId().toString().replaced("\n", "\n  ") << ",\n"
        "  \"serial-number\": \"" << serialNumber() << "\",\n"
        "  \"structure-label\": \"" << hex(structureLabel()) << "\",\n"
        "  \"locale\": " << locale().toString().replaced("\n", "\n  ") << "\n"
        "}";
}

} // namespace cc::isobus::pd
