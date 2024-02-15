/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Variant>
#include <cc/input>
#include <cc/str>

namespace cc {

Variant Variant::read(const String &s)
{
    if (s.startsWith('"') && s.endsWith('"'))
        return Variant{s.copy(1, s.count() - 1)};

    if (s.startsWith('[') && s.endsWith(']')) {
        List<String> sl = s.copy(1, s.count() - 1).split(',');
        List<Variant> vl;
        for (String &x: sl) {
            x.trim();
            vl.append(Variant::read(x));
        }
        return Variant{vl};
    }

    if (
        s.find('.') ||
        (!s.startsWith("0x") && (s.find('e') || s.find('E')))
    ) {
        double value = 0;
        if (scanNumber<double>(s, &value) == s.count())
            return Variant{value};
    }

    long value = 0;
    if (scanNumber<long>(s, &value) == s.count()) return Variant{value};
    if (s == "true" || s == "on") return Variant{true};
    if (s == "false" || s == "off") return Variant{false};

    return Variant{s};
}

} // namespace cc
