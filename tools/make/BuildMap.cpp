/*
 * Copyright (C) 2007-2014 Frank Mertens.
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
    buildPlanByPath_(BuildPlanByPath::create())
{}

void BuildMap::insert(String path, BuildPlan *plan)
{
    buildPlanByPath_->insert(path, plan);
}

bool BuildMap::lookup(String path, Ref<BuildPlan> *plan) const
{
    return buildPlanByPath_->lookup(path, plan);
}

String BuildMap::commonPrefix() const
{
    return buildPlanByPath_->commonPrefix('/');
}

BuildMap *buildMap() { return Singleton<BuildMap>::instance(); }

} // namespace fluxmake
