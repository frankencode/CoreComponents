/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
