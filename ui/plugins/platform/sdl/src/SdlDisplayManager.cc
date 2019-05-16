/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <SDL2/SDL.h>
#include <cc/Singleton>
#include <cc/ui/SdlDisplayManager>

namespace cc {
namespace ui {

SdlDisplayManager *SdlDisplayManager::instance()
{
    return Singleton<SdlDisplayManager>::instance();
}

SdlDisplayManager::SdlDisplayManager()
{
    const int n = getDisplayCount_();
    Ref<const Display> largestDisplay;
    #if 0
    bool allDesktop = true;
    bool allLandscape = true;
    bool allPortrait = true;
    bool allHighResolution = true;
    #endif
    for (int i = 0; i < n; ++i) {
        Ref<const Display> display = getDisplay_(i);
        if (!largestDisplay || display->diagonal() > largestDisplay->diagonal())
            largestDisplay = display;
        #if 0
        if (display->size()[1] < display->size()[0])
            allPortrait = false;
        if (display->size()[0] < display->size()[1])
            allLandscape = false;
        if (display->isHandheld())
            allDesktop = false;
        if (largestDisplay->dpi()[0] < 150)
            allHighResolution = false;
        #endif
    }

    if (largestDisplay) {
        double dpi = avg(largestDisplay->dpi());
        displayDensityRatio_ = dpi / 160;

        // limit the minimal text size to a minimum of recognizable pixels
        if (displayDensityRatio_ < 0.9) displayDensityRatio_ = 0.9;

        if (dpi < 200) {
            if (largestDisplay->size()[1] < largestDisplay->size()[0]) defaultFontSmoothing_ = FontSmoothing::RgbSubpixel;
            else defaultFontSmoothing_ = FontSmoothing::VrgbSubpixel;
        }
        else
            defaultFontSmoothing_ = FontSmoothing::Grayscale;

        #if 0
        if (allDesktop && !allHighResolution) {
            if (allLandscape) defaultFontSmoothing_ = FontSmoothing::RgbSubpixel;
            else if (allPortrait) defaultFontSmoothing_ = FontSmoothing::VrgbSubpixel;
            else defaultFontSmoothing_ = FontSmoothing::Grayscale;
        }
        else
            defaultFontSmoothing_ = FontSmoothing::Grayscale;
        #endif
    }
    else
        defaultFontSmoothing_ = FontSmoothing::Grayscale;
}

Ref<Display> SdlDisplayManager::getDisplay(int index) const
{
    return getDisplay_(index);
}

int SdlDisplayManager::getDisplayCount() const
{
    return getDisplayCount_();
}

Ref<Display> SdlDisplayManager::getDisplay_(int index)
{
    Ref<Display> display = Object::create<Display>();
    {
        float hdpi = 0;
        float vdpi = 0;
        if (SDL_GetDisplayDPI(index, NULL, &hdpi, &vdpi) != 0) {
            // TODO...
        }
        display->dpi = Size{ double(hdpi), double(vdpi) };
    }
    {
        SDL_Rect rect;
        if (SDL_GetDisplayBounds(index, &rect) != 0) {
            // TODO...
        }
        display->pos = Point{ double(rect.x), double(rect.y) };
        display->size = Size{ double(rect.w), double(rect.h) };
    }

    display->nativeMode->bind([=]{
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(index, &mode);
        return DisplayMode( Size{ double(mode.w), double(mode.h) }, mode.refresh_rate );
    });

    return display;
}

int SdlDisplayManager::getDisplayCount_()
{
    return SDL_GetNumVideoDisplays();
}

}} // namespace cc::ui
