/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "ConfigureCache.h"

namespace ccbuild {

ConfigureCache::ConfigureCache():
    cache_(Cache::create())
{}

bool ConfigureCache::lookup(String command, String *output) const
{
    return cache_->lookup(command, output);
}

void ConfigureCache::insert(String command, String output)
{
    cache_->insert(command, output);
}

ConfigureCache *configureCache() { return Singleton<ConfigureCache>::instance(); }

} // namespace ccbuild
