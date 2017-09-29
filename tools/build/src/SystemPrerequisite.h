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
    inline String origName() const { return origName_; }
    inline bool optional() const { return optional_; }
    inline bool cascade() const { return cascade_; }

    inline String compileFlagsConfigure() const { return compileFlagsConfigure_; }
    inline String linkFlagsConfigure() const { return linkFlagsConfigure_; }

    inline String versionConfigure() const { return versionConfigure_; }
    inline Version versionMin() const { return versionMin_; }
    inline void setVersionMin(Version newValue) { versionMin_ = newValue; }
    inline Version versionMax() const { return versionMax_; }
    inline void setVersionMax(Version newValue) { versionMax_ = newValue; }

    inline String configure() const { return configure_; }

    inline StringList *includeTest() const { return includeTest_; }

    inline bool autoConfigure() const { return autoConfigure_; }

private:
    SystemPrerequisite(String name);
    SystemPrerequisite(const MetaObject *object, BuildPlan *plan);

    String name_;
    String origName_;
    bool optional_;
    bool cascade_;

    String compileFlagsConfigure_;
    String linkFlagsConfigure_;

    String versionConfigure_;
    Version versionMin_;
    Version versionMax_;

    String configure_;

    Ref<StringList> includeTest_; // FIXME: obsolete starting from v0.14.0
    bool autoConfigure_;
};

typedef List< Ref<SystemPrerequisite> > SystemPrerequisiteList;
typedef Map< String, Ref<SystemPrerequisiteList> > SystemPrerequisitesByName;

} // namespace ccbuild
