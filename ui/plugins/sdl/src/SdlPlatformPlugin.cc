/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Registration>
#include <cc/ui/SdlApplication>
#include <cc/ui/SdlTimeMaster>
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

CC_REGISTRATION(SdlPlatformPlugin)

}} // namespace cc::ui
