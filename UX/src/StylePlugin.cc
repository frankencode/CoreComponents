/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/StylePlugin>
#include <cc/StyleManager>

namespace cc {

StylePlugin::StylePlugin(State *state):
    Object{state}
{
    StyleManager{}.registerPlugin(*this);
}

StylePlugin style()
{
    return StyleManager{}.activePlugin();
}

Theme theme()
{
    return StyleManager{}.activePlugin().theme();
}

} // namespace cc
