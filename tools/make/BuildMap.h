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

/** \brief keep track of build plans and target names
  */
class BuildMap: public Object
{
public:
    void insertPlan(String path, BuildPlan *plan);
    bool lookupPlan(String path, Ref<BuildPlan> *plan) const;
    String commonPrefix() const;

    bool registerLibrary(String name, String recipePath, String *exitingRecipePath);
    bool registerApplication(String name, String recipePath, String *existingRecipePath);

private:
    friend class Singleton<BuildMap>;
    BuildMap();

    typedef PrefixTree<char, Ref<BuildPlan> > BuildPlanByPath;
    Ref<BuildPlanByPath> buildPlanByPath_;

    typedef PrefixTree<char, String> RecipePathByTargetName;
    Ref<RecipePathByTargetName> libraries_;
    Ref<RecipePathByTargetName> applications_;
};

BuildMap *buildMap();

} // namespace fluxmake

#endif // FLUXMAKE_BUILDMAP_H
