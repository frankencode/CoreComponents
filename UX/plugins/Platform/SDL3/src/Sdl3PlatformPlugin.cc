/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sdl3PlatformPlugin>
#include <cc/Sdl3DisplayManager>
#include <cc/Sdl3TimeMaster>
#include <cc/Sdl3Application>
#include <cc/REGISTRATION>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_video.h>

namespace cc {

struct Sdl3PlatformPlugin::State final: public PlatformPlugin::State
{
    State():
        PlatformPlugin::State{"SDL3", 1}
    {

        SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");
        SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");

        SDL_SetHint(SDL_HINT_XINPUT_ENABLED, "1");
        SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");

        SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

        #ifndef NDEBUG
        // SDL_SetHint(SDL_HINT_EVENT_LOGGING, "2");
        #endif

        if (!SDL_Init(SDL_INIT_VIDEO)) throw Sdl3PlatformError{};
    }

    DisplayManager displayManager() const override
    {
        return Sdl3DisplayManager{};
    }

    TimeMaster timeMaster() const override
    {
        return Sdl3TimeMaster{};
    }

    Application application() const override
    {
        return Sdl3Application{};
    }
};

Sdl3PlatformPlugin::Sdl3PlatformPlugin():
    PlatformPlugin{new State}
{}

CC_REGISTRATION(Sdl3PlatformPlugin);

} // namespace cc
