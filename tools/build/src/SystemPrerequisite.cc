/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/str>
#include "SystemPrerequisite.h"

namespace ccbuild {

Ref<SystemPrerequisite> SystemPrerequisite::read(const MetaObject *object, BuildPlan *plan)
{
    return new SystemPrerequisite(object, plan);
}

SystemPrerequisite::SystemPrerequisite(const MetaObject *object, BuildPlan *plan):
    name_(object->value("name")),
    origName_(name_),
    optional_(object->value("optional")),
    includePathConfigure_(object->value("include-paths-configure")),
    libraryPathConfigure_(object->value("library-paths-configure")),
    compileFlagsConfigure_(object->value("compile-flags-configure")),
    linkFlagsConfigure_(object->value("link-flags-configure")),
    versionConfigure_(object->value("version-configure")),
    versionMin_(object->value("version-min")),
    versionMax_(object->value("version-max")),
    includeTest_(object->value("include-test"))
{
    BuildParameters::read(object, plan);
    if (name_ == "" && libraries_->count() == 1)
        name_ = libraries_->at(0);
    if (name_ == "") name_ = hex(uint64_t(this));
}

bool SystemPrerequisite::autoConfigure() const
{
    return
        origName_ != "" &&
        includePathConfigure_  == "" &&
        libraryPathConfigure_  == "" &&
        compileFlagsConfigure_ == "" &&
        linkFlagsConfigure_    == "" &&
        versionConfigure_      == ""
    ;
}

} // namespace ccbuild