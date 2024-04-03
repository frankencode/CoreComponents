/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PluginLoader>
#include <cc/PluginDir>

namespace cc {

PluginLoader::PluginLoader(const String &libraryPath, const String &group)
{
    String libraryName = libraryPath.baseName();
    String libraryInstallPath = libraryPath.cdUp();
    String pluginPath = libraryPath.cdUp() / libraryName / "plugins";
    if (group != "") pluginPath = pluginPath / group;
    PluginDir{pluginPath};
}

PluginLoader::PluginLoader(const String &libraryPath, const List<String> groups)
{
    for (const String &group: groups) {
        PluginLoader{libraryPath, group};
    }
}

} // namespace cc
