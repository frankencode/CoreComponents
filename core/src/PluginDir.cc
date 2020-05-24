/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/Map>
#include <cc/ValueSource>
#include <cc/Dir>
#include <cc/FileStatus>
#include <cc/File>
#include <cc/PluginManager>
#include <cc/PluginDir>

namespace cc {

Ref<PluginDir> PluginDir::open(const String &path)
{
    return (new PluginDir(path))->open();
}

PluginDir::PluginDir(const String &path):
    path_{path},
    loadedPlugins_{PluginByName::create()}
{}

PluginDir::~PluginDir()
{}

String PluginDir::path() const
{
    return path_;
}

Ref< Source<const Plugin *> > PluginDir::getLoadedPlugins() const
{
    return ValueSource<PluginByName, const Plugin *>::open(loadedPlugins_);
}

void PluginDir::onLoadError(const String &pluginPath, const String &errorMessage)
{
    ferr() << "Failed to load plugin " << pluginPath << ": " << errorMessage << nl;
}

PluginDir *PluginDir::open()
{
    Ref<Dir> dir = Dir::open(path_);
    for (String name; dir->read(&name);) {
        if (name == "." || name == "..") continue;
        String path = path_->extendPath(name);
        try {
            Ref<FileStatus> status = FileStatus::readHead(path);
            if (status->type() == FileType::Symlink) {
                path = File::readlink(path);
                if (path->isRelativePath())
                    path = path->absolutePathRelativeTo(path_);
            }
            else if (status->type() == FileType::Regular)
                ;
            else
                continue;

            Ref<Plugin> plugin = Plugin::load(path);
            loadedPlugins_->insert(plugin->name(), plugin);
            onLoaded(plugin);
        }
        catch (Exception &ex) {
            onLoadError(path, ex->message());
        }
    }
    PluginManager::instance()->registerPluginDir(this);
    return this;
}

} // namespace cc
