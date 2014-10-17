/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/str>
#include "SystemPrerequisite.h"

namespace fluxmake {

Ref<SystemPrerequisite> SystemPrerequisite::read(MetaObject *object, BuildPlan *plan)
{
    return new SystemPrerequisite(object, plan);
}

SystemPrerequisite::SystemPrerequisite(MetaObject *object, BuildPlan *plan)
    : name_(object->value("name")),
      value_(object->value("value")),
      description_(object->value("description")),
      optional_(object->value("optional")),
      testIncludes_(object->value("include-test")),
      testLibraries_(object->value("link-test"))
{
    if (name_ == "" && libraries_->count() == 1)
        name_ = libraries_->at(0);
    if (name_ == "") name_ = hex(uint64_t(this));
    BuildParameters::read(object, plan);
}

} // namespace fluxmake
