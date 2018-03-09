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

Ref<Display> SdlDisplayManager::getDisplay(int index) const
{
    Ref<Display> display = Object::create<Display>();
    {
        float hdpi = 0;
        float vdpi = 0;
        if (SDL_GetDisplayDPI(index, NULL, &hdpi, &vdpi) != 0) {
            // TODO...
        }
        display->dpi = Ratio{double(hdpi), double(vdpi)};
    }
    {
        SDL_Rect rect;
        if (SDL_GetDisplayBounds(index, &rect) != 0) {
            // TODO...
        }
        display->pos = Point{ double(rect.x), double(rect.y) };
        display->size = Size{ double(rect.w), double(rect.h) };
    }
    return display;
}

int SdlDisplayManager::getDisplayCount() const
{
    return SDL_GetNumVideoDisplays();
}

}} // namespace cc::ui
