/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TouchDeviceManager>
#include <cc/ui/PlatformPlugin>

namespace cc {
namespace ui {

TouchDeviceManager *TouchDeviceManager::instance()
{
    return PlatformPlugin::instance()->touchDeviceManager();
}

}} // namespace cc::ui
