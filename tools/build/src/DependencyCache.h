/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/string>
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

    static string cachePath(BuildPlan *plan);
    double cacheTime() const { return cacheTime_; }

    StringList *previousSources() const;

    bool lookup(const string &source, Ref<Module> *module);
    void insert(const string &source, Module *module);

private:
    DependencyCache(BuildPlan *plan);

    Ref<BuildPlan> buildPlan_;
    string cachePath_;
    double cacheTime_;
    typedef Map< string, Ref<Module> > Cache;
    Ref<Cache> cache_;

    Ref<StringList> previousSources_;
};

} // namespace ccbuild
