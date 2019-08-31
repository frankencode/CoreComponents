/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include "BuildPlan.h"
#include "BuildMap.h"

namespace ccbuild {

BuildMap *BuildMap::instance()
{
    return Singleton<BuildMap>::instance();
}

BuildMap::BuildMap():
    buildPlanByPath_{BuildPlanByPath::create()},
    libraries_{RecipePathByTargetName::create()},
    applications_{RecipePathByTargetName::create()}
{}

void BuildMap::insertPlan(String path, BuildPlan *plan)
{
    buildPlanByPath_->insert(path, plan);
}

bool BuildMap::lookupPlan(String path, Ref<BuildPlan> *plan) const
{
    return buildPlanByPath_->lookup(path, plan);
}

String BuildMap::commonPrefix() const
{
    return buildPlanByPath_->commonPrefix('/');
}

bool BuildMap::registerLibrary(String name, String recipePath, String *existingRecipePath)
{
    return libraries_->insert(name, recipePath, existingRecipePath);
}

bool BuildMap::registerApplication(String name, String recipePath, String *existingRecipePath)
{
    return applications_->insert(name, recipePath, existingRecipePath);
}

} // namespace ccbuild
