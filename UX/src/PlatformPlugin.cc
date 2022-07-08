/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
