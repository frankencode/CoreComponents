/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/cia/Selector>
#include <cc/Format>

namespace cc::cia {

String str(Selector selector)
{
    return
        Format{"%%:%%"}
            << hex(selector.index(), 4)
            << hex(selector.subIndex(), 2);
}

} // namespace cc::cia
