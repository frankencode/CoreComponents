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

SdlDisplayManager::SdlDisplayManager():
    DisplayManager(getDisplayDensityRatio())
{}

Ref<Display> SdlDisplayManager::getDisplay(int index) const
{
    return getDisplay_(index);
}

int SdlDisplayManager::getDisplayCount() const
{
    return getDisplayCount_();
}

double SdlDisplayManager::getDisplayDensityRatio()
{
    const int n = getDisplayCount_();
    Ref<Display> largestDisplay;
    for (int i = 0; i < n; ++i) {
        Ref<Display> display = getDisplay_(i);
        if (!largestDisplay || display->diagonal() > largestDisplay->diagonal())
            largestDisplay = display;
    }

    if (!largestDisplay) return 1;

    return (1 + 0.666 * !largestDisplay->isHandheld()) * avg(largestDisplay->dpi()) / 160;
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
