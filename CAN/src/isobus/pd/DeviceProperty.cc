/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/DeviceProperty>
#include <cc/isobus/DictionaryManager>
#include <cc/Format>

namespace cc::isobus::pd {

String DeviceProperty::toString() const
{
    TypeInfo typeInfo;
    DictionaryManager{}.processDataDictionary().lookup(valueType(), &typeInfo);

    Format f;

    f <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"object-id\": " << objectId() << ",\n"
        "  \"designator\": \"" << designator() << "\",\n";

    if (typeInfo) {
        f << "  \"value-type\": \"" << typeInfo.typeName() << "\",\n";
        if (typeInfo.scale() != 1 || typeInfo.unit() != "") {
            f << "  \"value-display\": \"" << typeInfo.scale() * value() << (typeInfo.unit() != "" ? " " : "") << typeInfo.unit() << "\",\n";
        }
        else {
            f << "  \"value\": " << value() << ",\n";
        }
    }
    else {
        f <<
            "  \"value-type\": " << valueType() << ",\n"
            "  \"value-type-is-proprietary\": " << isobus::pd::Dictionary::isProprietary(valueType()) << ",\n"
            "  \"value\": " << value() << ",\n";
    }

    f <<
        "  \"presentation-id\": " << presentationId() << "\n"
        "}";

    return f;
}

} // namespace cc::isobus::pd
