/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PlatformManager>
#include <cc/ui/DisplayManager>

namespace cc::ui {

DisplayManager::DisplayManager()
{
    *this = PlatformManager{}.activePlugin().displayManager();
}

} // namespace cc::ui
