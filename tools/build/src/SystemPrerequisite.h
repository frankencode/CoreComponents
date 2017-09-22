/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "BuildParameters.h"

namespace ccbuild {

using namespace cc;

class BuildPlan;

class SystemPrerequisite: public BuildParameters
{
public:
    static Ref<SystemPrerequisite> create(String name);
    static Ref<SystemPrerequisite> read(const MetaObject *object, BuildPlan *plan);

    inline String name() const { return name_; }
    inline bool optional() const { return optional_; }

    bool autoConfigure() const;

    inline String includePathConfigure() const { return includePathConfigure_; }
    inline String libraryPathConfigure() const { return libraryPathConfigure_; }

    inline String compileFlagsConfigure() const { return compileFlagsConfigure_; }
    inline String linkFlagsConfigure() const { return linkFlagsConfigure_; }

    inline String versionConfigure() const { return versionConfigure_; }
    inline Version versionMin() const { return versionMin_; }
    inline Version versionMax() const { return versionMax_; }

    inline StringList *includeTest() const { return includeTest_; }

private:
    SystemPrerequisite(String name);
    SystemPrerequisite(const MetaObject *object, BuildPlan *plan);

    String name_;
    String origName_;
    bool optional_;

    String includePathConfigure_;
    String libraryPathConfigure_;

    String compileFlagsConfigure_;
    String linkFlagsConfigure_;

    String versionConfigure_;
    Version versionMin_;
    Version versionMax_;

    Ref<StringList> includeTest_; // FIXME: remove include tests once 'configure' tools are functional
};

typedef List< Ref<SystemPrerequisite> > SystemPrerequisiteList;
typedef Map< String, Ref<SystemPrerequisiteList> > SystemPrerequisitesByName;

} // namespace ccbuild
