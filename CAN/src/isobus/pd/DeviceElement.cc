/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/DeviceElement>
#include <cc/Format>

namespace cc::isobus::pd {

String str(DeviceElement::ElementType type)
{
    String s;
    using Type = DeviceElement::ElementType;
    switch (type) {
        case Type::Device   : s = "isobus.device.element_type.device"; break;
        case Type::Function : s = "isobus.device.element_type.function"; break;
        case Type::Bin      : s = "isobus.device.element_type.bin"; break;
        case Type::Section  : s = "isobus.device.element_type.section"; break;
        case Type::Unit     : s = "isobus.device.element_type.unit"; break;
        case Type::Connector: s = "isobus.device.element_type.connector"; break;
        case Type::NavRef   : s = "isobus.device.element_type.navigation_reference"; break;
    };
    return s;
}

String DeviceElement::toString() const
{
    Format f;
    f <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"object-id\": " << objectId() << ",\n"
        "  \"designator\": \"" << designator() << "\",\n"
        "  \"element-type\": \"" << elementType() << "\",\n"
        "  \"element-number\": " << elementNumber() << ",\n"
        "  \"parent-id\": " << parentId() << ",\n";

    if (childrenCount() == 0) {
        f << "  \"children-ids\": []\n";
    }
    else {
        f << "  \"children-ids\": [\n";
        const uint16_t n = childrenCount();
        for (uint16_t i = 0; i < n; ++i) {
            f << "    " << childIdAt(i) << ((i == n -1) ? "\n" : ",\n");
        }
        f << "  ]\n";
    }
    f << "}";
    return f;
}

} // namespace cc::isobus::pd
