/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_BUILDMAP_H
#define FLUXMAKE_BUILDMAP_H

#include <flux/PrefixTree>

namespace flux {
template<class, class> class PrefixTree;
template<class> class Singleton;
}

namespace fluxmake {

using namespace flux;

class BuildPlan;

class BuildMap: public Object
{
public:
    void insert(String path, BuildPlan *plan);
    bool lookup(String path, Ref<BuildPlan> *plan) const;
    String commonPrefix() const;

private:
    friend class Singleton<BuildMap>;
    BuildMap();
    typedef PrefixTree<char, Ref<BuildPlan> > BuildPlanByPath;
    Ref<BuildPlanByPath> buildPlanByPath_;
};

BuildMap *buildMap();

} // namespace fluxmake

#endif // FLUXMAKE_BUILDMAP_H
