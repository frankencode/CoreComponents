/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cmath>
#include <cc/Registration>
#include <cc/Dir>
#include <cc/ui/design>
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
    String dejavuPath = "/usr/share/fonts/truetype/dejavu";
    if (Dir::exists(dejavuPath)) {
        FontManager::instance()->addPath(dejavuPath);
        defaultFont = Font("DejaVu Sans", sp(16));
        defaultFixedFont = Font("DejaVu Sans Mono", sp(16));
    }

    String notoPath = "/usr/share/fonts/truetype/noto";
    if (Dir::exists(notoPath)) {
        FontManager::instance()->addPath(notoPath);
        defaultFont = Font("Noto Sans", sp(16));
        defaultFixedFont = Font("Noto Mono", sp(16));
    }
}

CC_REGISTRATION(IndustrialStylePlugin)

}} // namespace cc::ui
