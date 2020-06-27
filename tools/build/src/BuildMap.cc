/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "BuildMap.h"
#include "BuildPlan.h"
#include <cc/Singleton>
#include <cc/types>

namespace ccbuild {

BuildMap *BuildMap::instance()
{
    return Singleton<BuildMap>::instance();
}

BuildMap::BuildMap()
{}

void BuildMap::insertPlan(const String &path, BuildPlan *plan)
{
    buildPlanByPath_->insert(path, plan);
}

bool BuildMap::lookupPlan(const String &path, Ref<BuildPlan> *plan) const
{
    return buildPlanByPath_->lookup(path, plan);
}

String BuildMap::commonPrefix() const
{
    if (buildPlanByPath_->count() == 0) return String{};

    return cc::commonPrefix(
        buildPlanByPath_->min()->key(),
        buildPlanByPath_->max()->key()
    );
}

bool BuildMap::registerLibrary(const String &name, const String &recipePath, String *existingRecipePath)
{
    Map<String, String>::Iterator it;
    if (!libraryNameByPath_->insert(name, recipePath, &it)) {
        *existingRecipePath = it->value();
        return false;
    }
    return true;
}

bool BuildMap::registerApplication(const String &name, const String &recipePath, String *existingRecipePath)
{
    Map<String, String>::Iterator it;
    if (!applicationNameByPath_->insert(name, recipePath, &it)) {
        *existingRecipePath = it->value();
        return false;
    }
    return true;
}

} // namespace ccbuild
