/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/SdlPlatformError>
#include <SDL2/SDL.h>

namespace cc {

String SdlPlatformError::message() const
{
    return SDL_GetError();
}

} // namespace cc
