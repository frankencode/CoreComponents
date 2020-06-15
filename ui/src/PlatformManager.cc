/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PlatformManager>
#include <cc/Singleton>
#include <cc/ValueSource>
#include <cc/assert>

namespace cc {
namespace ui {

PlatformManager *PlatformManager::instance()
{
    return Singleton<PlatformManager>::instance();
}

PlatformManager::PlatformManager()
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
    return ValueSource<Plugins::Instance, PlatformPlugin *>::open(plugins_);
}

PlatformPlugin *PlatformManager::activePlugin() const
{
    if (activePlugin_) return activePlugin_;
    CC_ASSERT2(plugins_->count() > 0, "No platform plugins available!");
    return plugins_->at(0)->value();
}

void PlatformManager::activatePlugin(PlatformPlugin *plugin)
{
    activePlugin_ = plugin;
}

}} // namespace cc::ui
