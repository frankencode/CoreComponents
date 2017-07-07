/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/exceptions>
#include <cc/String>

namespace cc { template<class> class Singleton; }

namespace cc {
namespace plugin {

class PluginLoadError: public Exception
{
public:
    PluginLoadError(String path);
    String path() const;
    String reason() const;
    virtual String message() const;

private:
    String path_;
};

class PluginManager: public Object
{
public:
    static PluginManager *instance();

    void loadPlugins(String dirPath);
    void loadPlugin(String libPath);

private:
    friend class Singleton<PluginManager>;
    PluginManager();
};

}} // namespace cc::plugin

