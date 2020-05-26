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

    void insertPlan(const string &path, BuildPlan *plan);
    bool lookupPlan(const string &path, Ref<BuildPlan> *plan) const;
    string commonPrefix() const;

    bool registerLibrary(const string &name, const string &recipePath, string *exitingRecipePath);
    bool registerApplication(const string &name, const string &recipePath, string *existingRecipePath);

private:
    friend class Singleton<BuildMap>;
    BuildMap();

    typedef PrefixTree<char, Ref<BuildPlan> > BuildPlanByPath;
    Ref<BuildPlanByPath> buildPlanByPath_;

    typedef PrefixTree<char, string> RecipePathByTargetName;
    Ref<RecipePathByTargetName> libraries_;
    Ref<RecipePathByTargetName> applications_;
};

} // namespace ccbuild
