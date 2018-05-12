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
#include <cc/ui/types>
#include <cc/ui/Application>
#include <cc/ui/FontManager>
#include <cc/ui/MaterialLight>
#include <cc/ui/MaterialDark>
#include <cc/ui/IndustrialStylePlugin>

namespace cc {
namespace ui {

IndustrialStylePlugin::IndustrialStylePlugin():
    StylePlugin("Industrial"),
    dayTheme_(Object::create<MaterialLight>()),
    nightTheme_(Object::create<MaterialDark>())
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
        defaultFont = Font("Noto Sans Display UI", sp(16));
        defaultFixedFont = Font("Noto Mono", sp(16));
    }

    String notoPath2 = "/usr/share/fonts/opentype/noto";
    if (Dir::exists(notoPath2))
        FontManager::instance()->addPath(notoPath2);
}

Theme *IndustrialStylePlugin::dayTheme() const
{
    return dayTheme_;
}

Theme *IndustrialStylePlugin::nightTheme() const
{
    return nightTheme_;
}

CC_REGISTRATION(IndustrialStylePlugin)

}} // namespace cc::ui
