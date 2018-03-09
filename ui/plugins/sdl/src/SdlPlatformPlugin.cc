/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Registration>
#include <cc/ui/SdlApplication>
#include <cc/ui/SdlTimeMaster>
#include <cc/ui/SdlDisplayManager>
#include <cc/ui/SdlPlatformPlugin>

namespace cc {
namespace ui {

SdlPlatformPlugin::SdlPlatformPlugin():
    PlatformPlugin("SDL", true)
{}

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

CC_REGISTRATION(SdlPlatformPlugin)

}} // namespace cc::ui
