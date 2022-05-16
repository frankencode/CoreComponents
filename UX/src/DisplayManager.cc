/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PlatformManager>
#include <cc/DisplayManager>

namespace cc {

DisplayManager::DisplayManager()
{
    *this = PlatformManager{}.activePlugin().displayManager();
}

} // namespace cc
