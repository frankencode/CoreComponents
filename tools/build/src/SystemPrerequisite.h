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
    static Ref<SystemPrerequisite> create(const String &name);
    static Ref<SystemPrerequisite> read(const MetaObject *object, BuildPlan *plan);

    String name() const { return name_; }
    String origName() const { return origName_; }
    bool optional() const { return optional_; }
    bool cascade() const { return cascade_; }
    void setCascade(bool newValue) { cascade_ = newValue; }

    String compileFlagsConfigure() const { return compileFlagsConfigure_; }
    String linkFlagsConfigure() const { return linkFlagsConfigure_; }

    String versionConfigure() const { return versionConfigure_; }
    Version versionMin() const { return versionMin_; }
    void setVersionMin(Version newValue) { versionMin_ = newValue; }
    Version versionMax() const { return versionMax_; }
    void setVersionMax(Version newValue) { versionMax_ = newValue; }

    String probe() const { return probe_; }
    String configure() const { return configure_; }

    bool autoConfigure() const { return autoConfigure_; }

private:
    SystemPrerequisite(const String &name);
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

    String probe_;
    String configure_;
    bool autoConfigure_;
};

typedef List< Ref<SystemPrerequisite> > SystemPrerequisiteList;
typedef Map< String, Ref<SystemPrerequisiteList> > SystemPrerequisitesByName;

} // namespace ccbuild
