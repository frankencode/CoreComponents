/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
