/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PlatformPlugin>
#include <cc/ui/PlatformManager>
#include <cc/ui/FtFontManager>

namespace cc::ui {

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

} // namespace cc::ui
