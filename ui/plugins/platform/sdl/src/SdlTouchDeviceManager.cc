/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <SDL2/SDL.h>
#include <cc/Singleton>
#include <cc/ui/SdlTouchDevice>
#include <cc/ui/SdlTouchDeviceManager>

namespace cc {
namespace ui {

SdlTouchDeviceManager *SdlTouchDeviceManager::instance()
{
    return Singleton<SdlTouchDeviceManager>::instance();
}

Ref<TouchDevice> SdlTouchDeviceManager::getTouchDevice(int index) const
{
    return Object::create<SdlTouchDevice>(SDL_GetTouchDevice(index));
}

int SdlTouchDeviceManager::getTouchDeviceCount() const
{
    return SDL_GetNumTouchDevices();
}

}} // namespace cc::ui
