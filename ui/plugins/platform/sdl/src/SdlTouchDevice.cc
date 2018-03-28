/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <SDL2/SDL.h>
#include <cc/ui/SdlTouchDevice>

namespace cc {
namespace ui {

TouchFinger SdlTouchDevice::getTouchFinger(int index) const
{
    SDL_Finger* f = SDL_GetTouchFinger(id_, index);
    if (f) return TouchFinger{ f->id, Point{double(f->x), double(f->y)}, double(f->pressure) };
    return TouchFinger{};
}

int SdlTouchDevice::getTouchFingerCount() const
{
    return SDL_GetNumTouchFingers(id_);
}

}} // namespace cc::ui
