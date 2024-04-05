/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/LanguageCommand>
#include <cc/j1939/AddressClaimManager>
#include <cc/j1939/CanId>
#include <cc/Format>

namespace cc::isobus {

CanFrame produce(uint8_t src, Locale locale)
{
    return
        CanFrame {
            j1939::CanId{}
            .prio(6)
            .pgn(LanguageCommand::pgn())
            .src(src)
        }
        (0, locale.data()(0))
        (1, locale.data()(1))
        (2, locale.data()(2))
        (3, locale.data()(3))
        (4, locale.data()(4))
        (5, locale.data()(5))
        (6, 0xFFu)
        (7, 0xFFu);
}

String LanguageCommand::toString() const
{
    Format f;
    f <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"pgn\": " << Message::pgn() << ",\n"
        "  \"src\": " << src() << ",\n";

    String srcName = j1939::AddressClaimManager{}.name(src());
    if (srcName != "") {
        f << "  \"src-name\": \"" << srcName << "\",\n";
    }

    f <<
        "  \"locale\": " << locale().toString().replaced("\n", "\n  ") << ",\n"
        "  \"prio\": " << prio() << "\n"
        "}";

    return f;
}

} // namespace cc::isobus
