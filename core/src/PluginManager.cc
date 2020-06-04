/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PluginManager>
#include <cc/Singleton>
#include <cc/IterationSource>

namespace cc {

PluginManager *PluginManager::instance()
{
    return Singleton<PluginManager>::instance();
}

PluginManager::PluginManager()
{}

PluginManager::~PluginManager()
{}

void PluginManager::registerPluginDir(PluginDir *pluginDir)
{
    pluginDirs_->append(pluginDir);
}

} // namespace cc
