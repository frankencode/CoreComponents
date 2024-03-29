/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SdlPlatformPlugin>
#include <cc/SdlDisplayManager>
#include <cc/SdlTimeMaster>
#include <cc/SdlApplication>
#include <cc/REGISTRATION>
#include <SDL2/SDL.h>

namespace cc {

struct SdlPlatformPlugin::State: public PlatformPlugin::State
{
    State():
        PlatformPlugin::State{"SDL", true}
    {
        #ifdef SDL_HINT_RENDER_BATCHING
        SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
        #endif

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
            // linear filtering causes artefacts with moving objects with OpenGL acceleration
            // therefore we might force nearest-neighbor filtering, which looks quite OK at the higher framerate

        SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");
        SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");

        SDL_SetHint(SDL_HINT_XINPUT_ENABLED, "1");
        SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");

        SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

        #ifndef NDEBUG
        // SDL_SetHint(SDL_HINT_EVENT_LOGGING, "1");
        #endif

        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) throw SdlPlatformError{};
    }

    DisplayManager displayManager() const override
    {
        return SdlDisplayManager{};
    }

    TimeMaster timeMaster() const override
    {
        return SdlTimeMaster{};
    }

    Application application() const override
    {
        return SdlApplication{};
    }
};

SdlPlatformPlugin::SdlPlatformPlugin():
    PlatformPlugin{new State}
{}

CC_REGISTRATION(SdlPlatformPlugin);

} // namespace cc
