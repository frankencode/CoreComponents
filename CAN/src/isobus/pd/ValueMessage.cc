/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/ValueMessage>
#include <cc/isobus/pd/TypeInfo>
#include <cc/isobus/pd/CondensedWorkState>
#include <cc/isobus/DictionaryManager>
#include <cc/j1939/AddressClaimManager>
#include <cc/Format>

namespace cc::isobus::pd {

String ValueMessage::toString() const
{
    TypeInfo typeInfo;
    DictionaryManager{}.processDataDictionary().lookup(valueType(), &typeInfo);

    Format f;

    f <<
        "{\n"
        "  \"type\": \"" << isobus::pd::Message::shortName() << "\",\n"
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

    f << "  \"element-number\": \"" << elementNumber() << "\",\n";

    f << "  \"ddi\": " << valueType() << ",\n";

    if (typeInfo) {
        f << "  \"value-type\": \"" << typeInfo.typeName() << "\",\n";
        if (typeInfo.scale() != 1 || typeInfo.unit() != "") {
            f << "  \"value-display\": \"" << typeInfo.scale() * value() << (typeInfo.unit() != "" ? " " : "") << typeInfo.unit() << "\",\n";
        }
        printValue(f);
    }
    else {
        f <<
            "  \"value-type\": " << valueType() << ",\n"
            "  \"value-type-is-proprietary\": " << isobus::pd::Dictionary::isProprietary(valueType()) << ",\n"
            "  \"value\": " << value() << ",\n";
    }

    f <<
        "  \"prio\": " << j1939::CanId{frame_}.prio() << "\n"
        "}";

    return f;
}

void ValueMessage::printValue(Format &f) const
{
    f << "  \"value\": " << value() << ",\n";
}

} // namespace cc::isobus::pd
