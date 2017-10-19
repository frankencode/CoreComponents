/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/Map>
#include <cc/Dir>
#include <cc/FileStatus>
#include <cc/PluginDir>

namespace cc {

Ref<PluginDir> PluginDir::load(String path)
{
    return (new PluginDir(path))->load();
}

PluginDir::PluginDir(String path):
    path_(path),
    loadedPlugins_(PluginByName::create())
{}

PluginDir::~PluginDir()
{}

Ref< Source< Ref<Plugin> > > PluginDir::getLoadedPlugins() const
{
    return loadedPlugins_->getAllValues();
}

void PluginDir::onLoadError(String pluginPath, String errorMessage)
{
    ferr() << "Failed to load plugin " << pluginPath << ": " << errorMessage << nl;
}

PluginDir *PluginDir::load()
{
    Ref<Dir> dir = Dir::open(path_);
    for (String name; dir->read(&name);) {
        String path = path_->extendPath(name);
        try {
            Ref<FileStatus> status = FileStatus::readUnresolved(path);
            if (status->type() == FileType::Regular) {
                Ref<Plugin> plugin = Plugin::load(path);
                loadedPlugins_->insert(plugin->name(), plugin);
                onLoaded(plugin);
            }
        }
        catch (Exception &ex) {
            onLoadError(path, ex->message());
        }
    }
    return this;
}

} // namespace cc
