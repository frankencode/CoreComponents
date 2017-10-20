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

PluginLoader::PluginLoader(String libraryPath)
{
    String libraryName = libraryPath->fileName()->split('.')->at(0);
    String libraryInstallPath = libraryPath->reducePath();
    String pluginPath = libraryPath->reducePath()->extendPath(libraryName)->extendPath("plugins");
    PluginDir::open(pluginPath);
}

} // namespace cc
