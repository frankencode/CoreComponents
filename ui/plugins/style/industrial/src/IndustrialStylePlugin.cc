/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/Registration>
#include <cc/ui/FontManager>
#include <cc/ui/IndustrialStylePlugin>

namespace cc {
namespace ui {

IndustrialStylePlugin::IndustrialStylePlugin():
    StylePlugin("Industrial")
{}

void IndustrialStylePlugin::activate()
{
    FontManager::instance()->addPath("/usr/share/fonts/truetype/dejavu/");
    defaultTextStyle = TextStyle::create(Font::select("DejaVu Sans", 16));
}

CC_REGISTRATION(IndustrialStylePlugin)

}} // namespace cc::ui
