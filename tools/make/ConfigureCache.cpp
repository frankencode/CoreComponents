/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "ConfigureCache.h"

namespace fluxmake {

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

} // namespace fluxmake
