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
    const double fontSize = sp(16);

    FontManager::instance()->addPath("/usr/share/fonts/truetype/dejavu");

    String robotoPath = "/usr/share/fonts/truetype/roboto/hinted/";
    if (Dir::exists(robotoPath)) {
        FontManager::instance()->addPath(robotoPath);
        defaultFont = Font("Roboto", fontSize);
    }
    else
        defaultFont = Font("DejaVu Sans", fontSize);

    defaultFixedFont = Font("DejaVu Sans Mono", fontSize);;
}

CC_REGISTRATION(IndustrialStylePlugin)

}} // namespace cc::ui
