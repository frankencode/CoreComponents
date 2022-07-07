/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PlatformManager>
#include <cc/DEBUG>

namespace cc {

PlatformManager::PlatformManager():
    Singleton{instance<State>()}
{}

void PlatformManager::setActivePlugin(const PlatformPlugin &plugin)
{
    #ifndef NDEBUG
    CC_DEBUG << plugin.name();
    #endif
    me().activePlugin_ = plugin;
}

void PlatformManager::registerPlugin(const PlatformPlugin &plugin)
{
    if (me().plugins_.insert(plugin.name(), plugin)) {
        if (plugin.isPlatformDefault())
            setActivePlugin(plugin);
    }
}

} // namespace cc
