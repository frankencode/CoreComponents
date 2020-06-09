/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/Map>

namespace cc { class File; }
namespace cc { namespace meta { class MetaObject; }}

namespace ccbuild {

using namespace cc;

class Module;
class BuildPlan;

class DependencyCache: public Object
{
public:
    static Ref<DependencyCache> create(BuildPlan *plan);
    ~DependencyCache();

    static String cachePath(BuildPlan *plan);
    double cacheTime() const { return cacheTime_; }

    StringList previousSources() const;

    bool lookup(const String &source, Ref<Module> *module);
    void insert(const String &source, Module *module);

private:
    DependencyCache(BuildPlan *plan);

    Ref<BuildPlan> buildPlan_;
    String cachePath_;
    double cacheTime_;
    typedef Map< String, Ref<Module> > Cache;
    Ref<Cache> cache_;

    StringList previousSources_;
};

} // namespace ccbuild
