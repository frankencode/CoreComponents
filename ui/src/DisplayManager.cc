/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PlatformPlugin>
#include <cc/ui/DisplayManager>

namespace cc {
namespace ui {

DisplayManager *DisplayManager::instance() { return PlatformPlugin::instance()->displayManager(); }

}} // namespace cc::ui
