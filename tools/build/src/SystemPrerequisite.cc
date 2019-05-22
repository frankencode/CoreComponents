/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/str>
#include "SystemPrerequisite.h"

namespace ccbuild {

Ref<SystemPrerequisite> SystemPrerequisite::create(const String &name)
{
    return new SystemPrerequisite{name};
}

Ref<SystemPrerequisite> SystemPrerequisite::read(const MetaObject *object, BuildPlan *plan)
{
    return new SystemPrerequisite{object, plan};
}

SystemPrerequisite::SystemPrerequisite(const String &name):
    name_{name},
    origName_{name},
    optional_{false},
    cascade_{false},
    autoConfigure_{true}
{
    includePaths_ = StringList::create();
    libraryPaths_ = StringList::create();
    libraries_ = StringList::create();
    customCompileFlags_ = StringList::create();
    customLinkFlags_ = StringList::create();
}

SystemPrerequisite::SystemPrerequisite(const MetaObject *object, BuildPlan *plan):
    name_{object->value("name")},
    origName_{name_},
    optional_{object->value("optional")},
    cascade_{object->value("cascade")},
    compileFlagsConfigure_{object->value("compile-flags-configure")},
    linkFlagsConfigure_{object->value("link-flags-configure")},
    versionConfigure_{object->value("version-configure")},
    versionMin_{object->value("version-min")},
    versionMax_{object->value("version-max")},
    probe_{object->value("probe")},
    configure_{object->value("configure")}
{
    BuildParameters::read(object, plan);
    if (name_ == "" && libraries_->count() == 1)
        name_ = libraries_->at(0);
    //if (name_ == "" && configure_ != "")
    //    name_ = configure_->baseName();
    if (name_ == "") name_ = hex(uint64_t(this));

    autoConfigure_ =
        origName_ != "" &&
        compileFlagsConfigure_ == "" &&
        linkFlagsConfigure_    == "" &&
        versionConfigure_      == "" &&
        customCompileFlags_    == "" &&
        customLinkFlags_       == "" &&
        probe_                 == "" &&
        configure_             == "";
}

} // namespace ccbuild
