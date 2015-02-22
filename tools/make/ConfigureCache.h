/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_CONFIGURECACHE_H
#define FLUXMAKE_CONFIGURECACHE_H

#include <flux/String>
#include <flux/Map>
#include <flux/Singleton>

namespace fluxmake {

using namespace flux;

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

} // namespace fluxmake

#endif // FLUXMAKE_CONFIGURECACHE_H
