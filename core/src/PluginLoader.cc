/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PluginDir>
#include <cc/PluginLoader>

namespace cc {

PluginLoader::PluginLoader(const string &libraryPath, const string &group)
{
    string libraryName = libraryPath->fileName()->split('.')->at(0);
    string libraryInstallPath = libraryPath->reducePath();
    string pluginPath = libraryPath->reducePath()->extendPath(libraryName)->extendPath("plugins");
    if (group != "") pluginPath = pluginPath->extendPath(group);
    PluginDir::open(pluginPath);
}

} // namespace cc
