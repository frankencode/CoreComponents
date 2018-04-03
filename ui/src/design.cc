/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/DisplayManager>
// #include <cc/ui/Style> // TODO...

namespace cc {
namespace ui {

double du(double x) { return DisplayManager::instance()->displayDensityRatio() * x; }

}} // namespace cc::ui
