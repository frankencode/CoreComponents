/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/PrefixTree>

namespace cc {
template<class, class> class PrefixTree;
template<class> class Singleton;
}

namespace ccbuild {

using namespace cc;

class BuildPlan;

/** \brief keep track of build plans and target names
  */
class BuildMap: public Object
{
public:
    static BuildMap *instance();

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

} // namespace ccbuild
