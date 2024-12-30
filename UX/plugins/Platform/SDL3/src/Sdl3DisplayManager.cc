/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sdl3DisplayManager>
#include <cc/Sdl3PlatformError>
#include <cc/debugging>
#include <SDL3/SDL_video.h>

namespace cc {

/** \todo add support for reading the SDL provided pixel format (SDL_DisplayMode::format) and making use of it for selecting the default font smoothing
  */
struct Sdl3DisplayManager::State final: public DisplayManager::State
{
    State()
    {
        Display largestDisplay;
        {
            int n = 0;
            SDL_DisplayID *displayIds = SDL_GetDisplays(&n);
            if (!displayIds) throw Sdl3PlatformError{};

            for (int i = 0; i < n; ++i)
            {
                Display display = getDisplay(displayIds[i]);
                displays_.append(display);
                if (!largestDisplay || display.diagonal() > largestDisplay.diagonal()) {
                    largestDisplay = display;
                }
            }

            SDL_free(displayIds);
        }

        if (largestDisplay)
        {
            double dpi = largestDisplay.dpi().avg();
            displayDensityRatio_ = dpi / 160;

            if (dpi < 200) {
                if (largestDisplay.size()[1] < largestDisplay.size()[0]) defaultFontSmoothing_ = FontSmoothing::RgbSubpixel;
                else defaultFontSmoothing_ = FontSmoothing::VrgbSubpixel;
            }
            else {
                defaultFontSmoothing_ = FontSmoothing::Grayscale;
            }

            refreshRate_ = largestDisplay.nativeMode().refreshRate();
        }
        else {
            defaultFontSmoothing_ = FontSmoothing::Grayscale;
        }
    }

    static Display getDisplay(const SDL_DisplayID &displayId)
    {
        #ifdef __ANDROID__
        const Size baseResolution { 160, 160 };
        #else
        const Size baseResolution { 96, 96 };
        #endif

        const float contentScale = SDL_GetDisplayContentScale(displayId);
        const Size dpi = contentScale * baseResolution;

        DisplayMode nativeMode;
        {
            const SDL_DisplayMode *mode = SDL_GetDesktopDisplayMode(displayId);
            if (!mode) throw Sdl3PlatformError{};

            nativeMode = DisplayMode {
                mode->pixel_density * Size {
                    static_cast<double>(mode->w),
                    static_cast<double>(mode->h)
                },
                static_cast<double>(mode->refresh_rate)
            };
        }

        Point pos;
        Size size;
        {
            SDL_Rect rect;
            if (!SDL_GetDisplayBounds(displayId, &rect)) throw Sdl3PlatformError{};
            pos = Point { static_cast<double>(rect.x), static_cast<double>(rect.y) };
            size = Size { static_cast<double>(rect.w), static_cast<double>(rect.h) };
        }

        return Display { pos, size, dpi, nativeMode };
    }
};

Sdl3DisplayManager::Sdl3DisplayManager():
    DisplayManager{instance<State>()}
{}

} // namespace cc
