/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/Selector>
#include <cc/str>
#include <cc/Format>

namespace cc {
namespace can {

String str(Selector selector)
{
    return
        Format("%%:%%")
            << hex(selector->index(), 4)
            << hex(selector->subIndex(), 2);
}

}} // namespace cc::can
