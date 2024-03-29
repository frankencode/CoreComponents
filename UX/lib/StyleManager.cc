/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PlatformManager>
#include <cc/StyleManager>
#include <cc/Process>
#include <cc/debugging>

namespace cc {

StyleManager::StyleManager():
    Singleton{instance<State>()}
{}

StyleManager::State::State():
    defaultStyleName_{Process::env("CCUI_STYLE", "Industrial")}
{
    PlatformManager{};
}

void StyleManager::setActivePlugin(const StylePlugin &plugin)
{
    #ifndef NDEBUG
    CC_DEBUG << plugin.name();
    #endif
    StylePlugin{plugin}.me().activate();
    me().activePlugin_ = plugin;
}

void StyleManager::registerPlugin(const StylePlugin &plugin)
{
    if (me().plugins_.insert(plugin.name(), plugin)) {
        if (plugin.name() == me().defaultStyleName_)
            setActivePlugin(plugin);
    }
}

} // namespace cc
