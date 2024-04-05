/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/Request>
#include <cc/j1939/DictionaryManager>
#include <cc/j1939/AddressClaimManager>
#include <cc/Format>

namespace cc::j1939 {

String Request::toString() const
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

    f << "  \"requested-pgn\": " << requestedPgn() << ",\n";

    MessageTypeInfo type;
    if (DictionaryManager{}.messageTypeDictionary().lookup(requestedPgn() << 8, &type)) {
        f << "  \"requested-type\": \"" << type.shortName() << "\",\n";
    }

    f <<
        "  \"prio\": " << prio() << "\n"
        "}";

    return f;
}

} // namespace cc::1939
