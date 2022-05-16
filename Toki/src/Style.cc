/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/Style>

namespace cc::toki {

void Style::State::realize()
{
    ruleName_ = value("rule").to<String>();
    ink_ = value("ink").to<Color>();
    paper_ = value("paper").to<Color>();
    bold_ = value("bold").to<bool>();
    italic_ = value("italic").to<bool>();
}

} // namespace cc::toki
