/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
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
