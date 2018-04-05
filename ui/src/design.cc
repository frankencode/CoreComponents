/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/DisplayManager>
#include <cc/ui/Application>
#include <cc/ui/design>

namespace cc {
namespace ui {

double dp(double x)
{
    return DisplayManager::instance()->displayDensityRatio() * x;
}

double sp(double x)
{
    return dp(x) * Application::instance()->textZoom();
}

}} // namespace cc::ui
