/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/Registration>
#include <cc/ui/Application>
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

    defaultFontUnscaled = Font("DejaVu Sans", 16);
    defaultFixedFontUnscaled = Font("DejaVu Sans Mono", 20);
    defaultMarginUnscaled = Size{0.5 * defaultFontUnscaled()->size()};
}

CC_REGISTRATION(IndustrialStylePlugin)

}} // namespace cc::ui
