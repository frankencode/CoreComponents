/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/StyleManager>
#include <cc/Singleton>
#include <cc/Map>
#include <cc/ValueSource>
#include <cc/Process>
#include <cc/assert>

namespace cc {
namespace ui {

StyleManager *StyleManager::instance()
{
    return Singleton<StyleManager>::instance();
}

StyleManager::StyleManager():
    plugins_{Plugins::create()},
    defaultStyleName_{Process::getEnv("CCUI_STYLE", "Industrial")}
{}

void StyleManager::registerPlugin(StylePlugin *plugin)
{
    if (plugins_->insert(plugin->name(), plugin)) {
        if (plugin->name() == defaultStyleName_)
            activatePlugin(plugin);
    }
}

bool StyleManager::getPlugin(const String &name, Ref<StylePlugin> *plugin) const
{
    return plugins_->lookup(name, plugin);
}

Ref< Source<StylePlugin *> > StyleManager::getAllPlugins() const
{
    return ValueSource<Plugins, StylePlugin *>::open(plugins_);
}

StylePlugin *StyleManager::activePlugin() const
{
    CC_ASSERT2(activePlugin_ != nullptr, "No style plugin activated!");
    return activePlugin_;
}

void StyleManager::activatePlugin(StylePlugin *plugin)
{
    plugin->activate();
    activePlugin_ = plugin;
}

}} // namespace cc::ui
