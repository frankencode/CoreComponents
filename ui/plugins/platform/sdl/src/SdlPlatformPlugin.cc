/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <SDL2/SDL.h>
#include <cc/exceptions>
#include <cc/Registration>
#include <cc/ui/SdlApplication>
#include <cc/ui/SdlTimeMaster>
#include <cc/ui/SdlDisplayManager>
#include <cc/ui/SdlTouchDeviceManager>
#include <cc/ui/SdlPlatformPlugin>

namespace cc {
namespace ui {

SdlPlatformPlugin::SdlPlatformPlugin():
    PlatformPlugin("SDL", true)
{
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        CC_DEBUG_ERROR(SDL_GetError());
}

Application *SdlPlatformPlugin::application() const
{
    return SdlApplication::instance();
}

TimeMaster *SdlPlatformPlugin::timeMaster() const
{
    return SdlTimeMaster::instance();
}

DisplayManager *SdlPlatformPlugin::displayManager() const
{
    return SdlDisplayManager::instance();
}

TouchDeviceManager *SdlPlatformPlugin::touchDeviceManager() const
{
    return SdlTouchDeviceManager::instance();
}

CC_REGISTRATION(SdlPlatformPlugin)

}} // namespace cc::ui
