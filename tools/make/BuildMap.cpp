/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton>
#include "BuildPlan.h"
#include "BuildMap.h"

namespace fluxmake {

BuildMap::BuildMap():
    buildPlanByPath_(BuildPlanByPath::create()),
    libraries_(RecipePathByTargetName::create()),
    applications_(RecipePathByTargetName::create())
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

BuildMap *buildMap() { return Singleton<BuildMap>::instance(); }

} // namespace fluxmake
