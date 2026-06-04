/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/Sdl3PlatformError>
#include <SDL3/SDL_error.h>

namespace cc {

String Sdl3PlatformError::message() const
{
    return SDL_GetError();
}

} // namespace cc
