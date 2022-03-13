/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PlatformManager>
#include <cc/DEBUG>

namespace cc::ui {

PlatformManager::PlatformManager():
    Singleton{instance<State>()}
{}

void PlatformManager::setActivePlugin(const PlatformPlugin &plugin)
{
    CC_DEBUG << plugin.name();
    me().activePlugin_ = plugin;
}

void PlatformManager::registerPlugin(const PlatformPlugin &plugin)
{
    if (me().plugins_.insert(plugin.name(), plugin)) {
        if (plugin.isPlatformDefault())
            setActivePlugin(plugin);
    }
}

} // namespace cc::ui
