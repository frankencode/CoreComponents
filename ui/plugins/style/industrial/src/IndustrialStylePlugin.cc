/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cmath>
#include <cc/Registration>
#include <cc/Bundle>
#include <cc/Dir>
#include <cc/ui/types>
#include <cc/ui/Application>
#include <cc/ui/FontManager>
#include <cc/ui/MaterialLight>
#include <cc/ui/MaterialDark>
#include <cc/ui/GlyphVisual>
#include <cc/ui/IndustrialStylePlugin>

namespace cc {
namespace ui {

IndustrialStylePlugin::IndustrialStylePlugin():
    StylePlugin{"Industrial"},
    dayTheme_{Object::create<MaterialLight>()},
    nightTheme_{Object::create<MaterialDark>()},
    iconFont_{"Icons", std::ceil(dp(24))}
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
        auto families = FontManager::instance()->getFontFamilies();
        for (auto family: families) {
            if (family->name() == "Noto Sans Display") {
                defaultFont = Font("Noto Sans Display", sp(16));
                break;
            }
            if (family->name() == "Noto Sans UI") {
                defaultFont = Font("Noto Sans UI", sp(16));
                break;
            }
        }
        for (auto family: families) {
            if (family->name() == "Noto Mono") {
                defaultFixedFont = Font("Noto Mono", sp(16));
                break;
            }
        }
    }

    String notoPath2 = "/usr/share/fonts/opentype/noto";
    if (Dir::exists(notoPath2))
        FontManager::instance()->addPath(notoPath2);

    FontManager::instance()->addPath(CC_BUNDLE_LOOKUP("icons"));
}

Theme *IndustrialStylePlugin::dayTheme() const
{
    return dayTheme_;
}

Theme *IndustrialStylePlugin::nightTheme() const
{
    return nightTheme_;
}

Ref<IconVisual> IndustrialStylePlugin::getIconVisual(Icon icon) const
{
    auto visual = GlyphVisual::create(static_cast<uchar_t>(icon), iconFont_);
    visual->ink->bind([=]{ return theme()->primaryTextColor(); });
    return visual;
}

CC_REGISTRATION(IndustrialStylePlugin)

}} // namespace cc::ui
