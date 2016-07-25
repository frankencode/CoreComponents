/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/Map>
#include <cc/Singleton>

namespace ccbuild {

using namespace cc;

class ConfigureCache: public Object
{
public:
    bool lookup(String command, String *output) const;
    void insert(String command, String output);

private:
    friend class Singleton<ConfigureCache>;

    ConfigureCache();

    typedef Map<String, String> Cache;
    Ref<Cache> cache_;
};

ConfigureCache *configureCache();

} // namespace ccbuild

