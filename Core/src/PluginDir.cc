/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PluginDir>
#include <cc/Dir>
#include <cc/FileStatus>
#include <cc/File>
#include <cc/stdio>

namespace cc {

PluginDir::PluginDir(const String &path):
    Object{new State{path}}
{
    Dir dir{me().path_};
    for (const String &name: dir) {
        String path = me().path_ / name;
        try {
            FileStatus status{path, false};
            if (status.type() == FileType::Symlink) {
                path = File::readlink(path);
                if (path.isRelativePath())
                    path = me().path_.cd(path);
            }
            else if (status.type() == FileType::Regular)
                ;
            else
                continue;

            Plugin plugin{path};
            me().loadedPlugins_.insert(plugin.name(), plugin);
            onLoaded(plugin);
        }
        catch (Exception &ex) {
            onError(path, ex.message());
        }
    }

    getPluginDirs() << *this;
}

void PluginDir::onError(const String &pluginPath, const String &errorMessage)
{
    ferr() << "Failed to load plugin " << pluginPath << ": " << errorMessage << nl;
}

} // namespace cc
