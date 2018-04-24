/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Application>
#include <cc/ui/StyleManager>
#include <cc/ui/StylePlugin>

namespace cc {
namespace ui {

StylePlugin *StylePlugin::instance()
{
    return StyleManager::instance()->activePlugin();
}

StylePlugin::StylePlugin(String name):
    name_(name)
{}

void StylePlugin::init()
{
    theme = dayTheme();
    StyleManager::instance()->registerPlugin(this);

    Application *app = Application::instance();

    app->defaultFont->bind([=]{
        return Font(sp(
            theme()->defaultFontSize() * app->textZoom()
        ));
    });

    Application::instance()->smallFont->bind([=]{
        return Font(sp(
            theme()->smallFontSize() * app->textZoom()
        ));
    });
}

Size StylePlugin::defaultMargin(double fontSize) const
{
    return 0.5 * Size{fontSize, fontSize};
}

}} // namespace cc::ui
