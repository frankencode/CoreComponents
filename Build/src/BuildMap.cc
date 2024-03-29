/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/BuildMap>
#include <cc/Map>

namespace cc::build {

struct BuildMap::State: public Object::State
{
    Map<String, BuildPlan> buildPlanByPath_;
    Map<String, String> libraryNameByPath_;
    Map<String, String> applicationNameByPath_;
};

BuildMap::BuildMap()
{
    initOnce<State>();
}

void BuildMap::insertPlan(const String &path, const BuildPlan &plan)
{
    me().buildPlanByPath_.insert(path, plan);
}

bool BuildMap::lookupPlan(const String &path, Out<BuildPlan> plan) const
{
    return me().buildPlanByPath_.lookup(path, &plan);
}

String BuildMap::commonPrefix() const
{
    if (me().buildPlanByPath_.count() == 0) return String{};

    return cc::commonPrefix(
        me().buildPlanByPath_.min().key(),
        me().buildPlanByPath_.max().key()
    );
}

bool BuildMap::registerLibrary(const String &name, const String &recipePath, Out<String> existingRecipePath)
{
    Locator pos;
    if (!me().libraryNameByPath_.insert(name, recipePath, &pos)) {
        existingRecipePath = me().libraryNameByPath_.at(pos).value();
        return false;
    }
    return true;
}

bool BuildMap::registerApplication(const String &name, const String &recipePath, Out<String> existingRecipePath)
{
    Locator pos;
    if (!me().applicationNameByPath_.insert(name, recipePath, &pos)) {
        existingRecipePath = me().applicationNameByPath_.at(pos).value();
        return false;
    }
    return true;
}

bool BuildMap::lookupPlanForHeader(const String &headerPath, Out<BuildPlan> plan) const
{
    List<String> parts = headerPath.split('/');
    while (parts.count() > 0 && parts.last() != "include") {
        parts.popBack();
    }
    if (parts.count() == 0) return false;
    parts.popBack();
    return lookupPlan(parts.join('/'), &plan);
}

const BuildMap::State &BuildMap::me() const
{
    return Object::me.as<State>();
}

BuildMap::State &BuildMap::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
