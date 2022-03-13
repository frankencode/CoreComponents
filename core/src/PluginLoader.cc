/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PluginLoader>
#include <cc/PluginDir>

namespace cc {

PluginLoader::PluginLoader(const String &libraryPath, const String &group)
{
    String libraryName = libraryPath.fileName().split('.').at(0);
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
