/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/LocalizationLabelReply>
#include <cc/j1939/AddressClaimManager>
#include <cc/Format>

namespace cc::isobus::pd {

String LocalizationLabelReply::toString() const
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

    Locale l = locale();
    f <<
        "  \"language-code\": \"" << l.languageCode() << "\",\n"
        "  \"time-format\": \"" << l.timeFormat() << "\",\n"
        "  \"date-format\": \"" << l.dateFormat() << "\",\n"
        "  \"decimal-point\": \"" << l.decimalSymbol() << "\",\n"
        "  \"mass-units\": \"" << l.massUnits() << "\",\n"
        "  \"volume-units\": \"" << l.volumeUnits() << "\",\n"
        "  \"area-units\": \"" << l.areaUnits() << "\",\n"
        "  \"distance-units\": \"" << l.distanceUnits() << "\",\n"
        "  \"force-units\": \"" << l.forceUnits() << "\",\n"
        "  \"pressure-units\": \"" << l.pressureUnits() << "\",\n"
        "  \"temperature-units\": \"" << l.temperatureUnits() << "\",\n"
        "  \"default-units\": \"" << l.defaultUnits() << "\",\n"
        "  \"prio\": " << prio() << "\n"
        "}";

    return f;
}

} // namespace cc::isobus::pd
