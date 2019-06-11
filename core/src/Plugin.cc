/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <dlfcn.h>
#include <cc/Plugin>

namespace cc {

String PluginLoadError::message() const
{
    return dlerror();
}

Ref<Plugin> Plugin::load(String path)
{
    return (new Plugin(path))->load();
}

Plugin::Plugin(String path):
    path_(path),
    name_(path->baseName()),
    handle_(0)
{}

Plugin *Plugin::load()
{
    handle_ = dlopen(path_, RTLD_LAZY|RTLD_LOCAL);
    if (!handle_) throw PluginLoadError{};
    return this;
}

void Plugin::reload()
{
    if (dlclose(handle_) != 0)
        throw PluginLoadError{};
    load();
}

void Plugin::close()
{
    if (handle_) {
        dlclose(handle_);
        handle_ = 0;
    }
}

} // namespace cc
