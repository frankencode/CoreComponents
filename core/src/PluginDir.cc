/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PluginDir>
#include <cc/stdio>
#include <cc/ValueSource>
#include <cc/Dir>
#include <cc/FileStatus>
#include <cc/File>
#include <cc/PluginManager>

namespace cc {

Ref<PluginDir> PluginDir::open(const String &path)
{
    return (new PluginDir{path})->open();
}

PluginDir::PluginDir(const String &path):
    path_{path}
{}

PluginDir::~PluginDir()
{}

String PluginDir::path() const
{
    return path_;
}

void PluginDir::onLoadError(const String &pluginPath, const String &errorMessage)
{
    ferr() << "Failed to load plugin " << pluginPath << ": " << errorMessage << nl;
}

PluginDir *PluginDir::open()
{
    Dir dir{path_};
    for (const auto &name: dir) {
        if (name == "." || name == "..") continue;
        auto path = path_->extendPath(name);
        try {
            FileStatus status{path, false};
            if (status->type() == FileType::Symlink) {
                path = File::readlink(path);
                if (path->isRelativePath())
                    path = path->absolutePathRelativeTo(path_);
            }
            else if (status->type() == FileType::Regular)
                ;
            else
                continue;

            auto plugin = Plugin::load(path);
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
