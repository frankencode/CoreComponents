/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Map>
#include <cc/String>

namespace cc { template<class> class Singleton; }

namespace ccbuild {

using namespace cc;

class BuildPlan;

/** \brief keep track of build plans and target names
  */
class BuildMap: public Object
{
public:
    static BuildMap *instance();

    void insertPlan(const String &path, BuildPlan *plan);
    bool lookupPlan(const String &path, Ref<BuildPlan> *plan) const;
    String commonPrefix() const;

    bool registerLibrary(const String &name, const String &recipePath, String *exitingRecipePath);
    bool registerApplication(const String &name, const String &recipePath, String *existingRecipePath);

private:
    friend class Singleton<BuildMap>;
    BuildMap();

    Map<String, Ref<BuildPlan> > buildPlanByPath_;
    Map<String, String> libraryNameByPath_;
    Map<String, String> applicationNameByPath_;
};

} // namespace ccbuild
