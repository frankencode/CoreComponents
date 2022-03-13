/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Plugin>
#include <dlfcn.h>

namespace cc {

Plugin::Plugin(const String &path):
    Object{new State{path, path.baseName()}}
{
    load();
}

void Plugin::load()
{
    me().handle_ = dlopen(me().path_, RTLD_LAZY|RTLD_LOCAL);
    if (!me().handle_) throw PluginError{};
}

void Plugin::reload()
{
    if (dlclose(me().handle_) != 0) throw PluginError{};
    load();
}

void Plugin::close()
{
    if (me().handle_) {
        dlclose(me().handle_);
        me().handle_ = nullptr;
    }
}

String PluginError::message() const
{
    return dlerror();
}

} // namespace cc
