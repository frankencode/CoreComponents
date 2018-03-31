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
            Font::select("Sans", 20 + Application::instance()->textZoom())
        );
    });

    defaultMonoTextStyle->bind([=]{
        return TextStyle::create(
            Font::select("Mono", 16 + Application::instance()->textZoom())
        );
    });

    defaultMonoBoldTextStyle->bind([=]{
        return TextStyle::create(
            Font::select("Mono", 16 + Application::instance()->textZoom(), Weight::Bold)
        );
    });

    defaultTextMargin->bind([=]{
        return Size { 0.5 * defaultTextStyle()->font()->size() };
    });
}

CC_REGISTRATION(IndustrialStylePlugin)

}} // namespace cc::ui
