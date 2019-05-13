/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/assert>
#include <cc/Singleton>
#include <cc/Map>
#include <cc/ui/PlatformManager>

namespace cc {
namespace ui {

PlatformManager *PlatformManager::instance()
{
    return Singleton<PlatformManager>::instance();
}

PlatformManager::PlatformManager():
    plugins_{Plugins::create()}
{}

void PlatformManager::registerPlugin(PlatformPlugin *plugin)
{
    if (plugins_->insert(plugin->name(), plugin)) {
        if (plugin->isPlatformDefault())
            activatePlugin(plugin);
    }
}

bool PlatformManager::getPlugin(const String &name, PlatformPlugin **plugin) const
{
    return plugins_->lookup(name, plugin);
}

Ref< Source<PlatformPlugin *> > PlatformManager::getAllPlugins() const
{
    return plugins_->getAllValues<PlatformPlugin *>();
}

PlatformPlugin *PlatformManager::activePlugin() const
{
    if (activePlugin_) return activePlugin_;
    CC_ASSERT2(plugins_->count() > 0, "No platform plugins available!");
    return plugins_->valueAt(0);
}

void PlatformManager::activatePlugin(PlatformPlugin *plugin)
{
    activePlugin_ = plugin;
}

}} // namespace cc::ui
