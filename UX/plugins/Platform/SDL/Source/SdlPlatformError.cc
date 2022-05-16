/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
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
