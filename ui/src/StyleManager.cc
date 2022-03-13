/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PlatformManager>
#include <cc/ui/StyleManager>
#include <cc/Process>
#include <cc/DEBUG>

namespace cc::ui {

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
    CC_DEBUG << plugin.name();
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

} // namespace cc::ui
