/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PlatformPlugin>
#include <cc/PlatformManager>
#include <cc/FtFontManager>

namespace cc {

FontManager PlatformPlugin::State::fontManager() const
{
    return FtFontManager{};
}

PlatformPlugin::PlatformPlugin(State *state):
    Object{state}
{
    PlatformManager{}.registerPlugin(*this);
}

PlatformPlugin platform()
{
    return PlatformManager{}.activePlugin();
}

} // namespace cc
