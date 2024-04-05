/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/i18n>
#include <cc/str>

namespace cc::isobus {

String str(TimeFormat timeFormat)
{
    String s;
    switch (timeFormat) {
        case TimeFormat::Std24:    s = "24h"; break;
        case TimeFormat::AmPm:     s = "AM/PM"; break;
        case TimeFormat::Reserved: s = "?"; break;
        case TimeFormat::Unspec:   s = ""; break;
    };
    return s;
}

String str(DateFormat dateFormat)
{
    String s;
    switch (dateFormat) {
        case DateFormat::DDMMYYYY: s = "DDMMYYYY"; break;
        case DateFormat::DDYYYYMM: s = "DDYYYYMM"; break;
        case DateFormat::MMYYYYDD: s = "MMYYYYDD"; break;
        case DateFormat::MMDDYYYY: s = "MMDDYYYY"; break;
        case DateFormat::YYYYMMDD: s = "YYYYMMDD"; break;
        case DateFormat::YYYYDDMM: s = "YYYYDDMM"; break;
        default: s = cc::str(static_cast<uint8_t>(dateFormat));
    };
    return s;
}

String str(DecimalSymbol decimalSymbol)
{
    String s;
    switch (decimalSymbol) {
        case DecimalSymbol::Comma: s = ","; break;
        case DecimalSymbol::Point: s = "."; break;
        case DecimalSymbol::Reserved: s = "?"; break;
        case DecimalSymbol::Unspec: s = ""; break;
    };
    return s;
}

String str(Units units)
{
    String s;
    switch (units) {
        case Units::Metric  : s = "Metric"; break;
        case Units::Imperial: s = "Imperial"; break;
        case Units::Other   : s = "US"; break;
        case Units::Unspec  : s = ""; break;
    };
    return s;
}

} // namespace cc::isobus
