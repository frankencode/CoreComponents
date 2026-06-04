/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/PlatformManager>
#include <cc/debugging>

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
        if (!me().activePlugin_ || me().activePlugin_.ranking() < plugin.ranking())
            setActivePlugin(plugin);
    }
}

} // namespace cc
