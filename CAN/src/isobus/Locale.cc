/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/Locale>
#include <cc/Format>

namespace cc::isobus {

String Locale::toString() const
{
    return Format{} <<
        "{\n"
        "  \"type\": \"isobus.locale\",\n"
        "  \"language-code\": \"" << languageCode() << "\",\n"
        "  \"time-format\": \"" << timeFormat() << "\",\n"
        "  \"date-format\": \"" << dateFormat() << "\",\n"
        "  \"decimal-point\": \"" << decimalSymbol() << "\",\n"
        "  \"mass-units\": \"" << massUnits() << "\",\n"
        "  \"volume-units\": \"" << volumeUnits() << "\",\n"
        "  \"area-units\": \"" << areaUnits() << "\",\n"
        "  \"distance-units\": \"" << distanceUnits() << "\",\n"
        "  \"force-units\": \"" << forceUnits() << "\",\n"
        "  \"pressure-units\": \"" << pressureUnits() << "\",\n"
        "  \"temperature-units\": \"" << temperatureUnits() << "\",\n"
        "  \"default-units\": \"" << defaultUnits() << "\"\n"
        "}";
}

} // namespace cc::isobus
