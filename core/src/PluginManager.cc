/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/List>
#include <cc/PluginManager>

namespace cc {

PluginManager *PluginManager::instance()
{
    return Singleton<PluginManager>::instance();
}

Ref< Source< Ref<PluginDir> > > PluginManager::getPluginDirs() const
{
    return pluginDirs_->getAll();
}

PluginManager::PluginManager():
    pluginDirs_(PluginDirs::create())
{}

PluginManager::~PluginManager()
{}

void PluginManager::registerPluginDir(PluginDir *pluginDir)
{
    pluginDirs_->append(pluginDir);
}

} // namespace cc
