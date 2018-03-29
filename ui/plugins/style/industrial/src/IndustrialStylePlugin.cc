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

    defaultTextStyle->bind([=]{
        return TextStyle::create(
            Font::select("DejaVu Sans", 20 + Application::instance()->textZoom())
        );
    });

    defaultTextMargin->bind([=]{
        return Size { 0.5 * defaultTextStyle()->font()->size() };
    });
}

CC_REGISTRATION(IndustrialStylePlugin)

}} // namespace cc::ui
