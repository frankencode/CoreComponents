/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <dlfcn.h>
#include <cc/exceptions>
#include <cc/Singleton>
#include <cc/Dir>
#include <cc/Format>
#include "PluginManager.h"

namespace cc {
namespace plugin {

PluginLoadError::PluginLoadError(String path):
    path_(path)
{}

String PluginLoadError::path() const
{
    return path_;
}

String PluginLoadError::reason() const
{
    return dlerror();
}

String PluginLoadError::message() const
{
    return Format("Failed to load plugin %%: %%") << path_ << reason();
}

PluginManager *PluginManager::instance()
{
    return Singleton<PluginManager>::instance();
}

PluginManager::PluginManager():
    pluginPaths_(StringList::create())
{}

void PluginManager::loadPlugins(String dirPath)
{
    Ref<Dir> dir = Dir::open(dirPath);
    for (String name; dir->read(&name);) {
        if (name == "." || name == "..") continue;
        String path = dirPath->expandPath(name);
        void *handle = dlopen(path, RTLD_LAZY);
        if (!handle)
            throw PluginLoadError(path);
    }
}

}} // namespace cc::plugin
