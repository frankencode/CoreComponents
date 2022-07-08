/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SdlDisplayManager>
#include <cc/SdlPlatformError>
#include <SDL2/SDL.h>
#include <cc/DEBUG>

namespace cc {

struct SdlDisplayManager::State final: public DisplayManager::State
{
    State()
    {
        const int n = SDL_GetNumVideoDisplays();
        Display largestDisplay;
        for (int i = 0; i < n; ++i) {
            Display display = getDisplay(i);
            displays_.append(display);
            if (largestDisplay && display.diagonal() > largestDisplay.diagonal())
                largestDisplay = display;
        }

        if (largestDisplay) {
            double dpi = largestDisplay.dpi().avg();
            displayDensityRatio_ = dpi / 160;

            // limit the minimal text size to a minimum of recognizable pixels
            if (displayDensityRatio_ < 0.9) displayDensityRatio_ = 0.9;

            if (dpi < 200) {
                if (largestDisplay.size()[1] < largestDisplay.size()[0]) defaultFontSmoothing_ = FontSmoothing::RgbSubpixel;
                else defaultFontSmoothing_ = FontSmoothing::VrgbSubpixel;
            }
            else
                defaultFontSmoothing_ = FontSmoothing::Grayscale;

            refreshRate_ = largestDisplay.nativeMode().refreshRate();
        }
        else
            defaultFontSmoothing_ = FontSmoothing::Grayscale;
    }
};

SdlDisplayManager::SdlDisplayManager():
    DisplayManager{instance<State>()}
{}

Display SdlDisplayManager::getDisplay(int index)
{
    Point pos;
    Size size;
    Size dpi;
    DisplayMode nativeMode;

    try {
        float hdpi = 0;
        float vdpi = 0;
        if (SDL_GetDisplayDPI(index, NULL, &hdpi, &vdpi) != 0) throw SdlPlatformError{};
        dpi = Size{ double(hdpi), double(vdpi) };
    }
    catch (Exception &error)
    {
        CC_INSPECT(error);
        CC_INSPECT(index);
        dpi = Size{150, 150};
    }

    {
        SDL_Rect rect;
        if (SDL_GetDisplayBounds(index, &rect) != 0) throw SdlPlatformError{};
        pos = Point{ double(rect.x), double(rect.y) };
        size = Size{ double(rect.w), double(rect.h) };
    }

    {
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(index, &mode);
        nativeMode = DisplayMode{ Size{ double(mode.w), double(mode.h) }, double(mode.refresh_rate) };
    }

    return Display{pos, size, dpi, nativeMode};
}

} // namespace cc
