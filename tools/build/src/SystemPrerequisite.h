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
    static Ref<SystemPrerequisite> create(const string &name);
    static Ref<SystemPrerequisite> read(const MetaObject *object, BuildPlan *plan);

    string name() const { return name_; }
    string origName() const { return origName_; }
    bool optional() const { return optional_; }
    bool cascade() const { return cascade_; }
    void setCascade(bool newValue) { cascade_ = newValue; }

    string compileFlagsConfigure() const { return compileFlagsConfigure_; }
    string linkFlagsConfigure() const { return linkFlagsConfigure_; }

    string versionConfigure() const { return versionConfigure_; }
    Version versionMin() const { return versionMin_; }
    void setVersionMin(Version newValue) { versionMin_ = newValue; }
    Version versionMax() const { return versionMax_; }
    void setVersionMax(Version newValue) { versionMax_ = newValue; }

    string probe() const { return probe_; }
    string configure() const { return configure_; }

    bool autoConfigure() const { return autoConfigure_; }

private:
    SystemPrerequisite(const string &name);
    SystemPrerequisite(const MetaObject *object, BuildPlan *plan);

    string name_;
    string origName_;
    bool optional_;
    bool cascade_;

    string compileFlagsConfigure_;
    string linkFlagsConfigure_;

    string versionConfigure_;
    Version versionMin_;
    Version versionMax_;

    string probe_;
    string configure_;
    bool autoConfigure_;
};

typedef List< Ref<SystemPrerequisite> > SystemPrerequisiteList;
typedef Map< string, Ref<SystemPrerequisiteList> > SystemPrerequisitesByName;

} // namespace ccbuild
