/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/str.h>
#include "SystemPrerequisite.h"

namespace fluxmake
{

Ref<SystemPrerequisite> SystemPrerequisite::read(YasonObject *object, BuildPlan *plan)
{
	return new SystemPrerequisite(object, plan);
}

SystemPrerequisite::SystemPrerequisite(YasonObject *object, BuildPlan *plan)
	: name_(object->value("name")),
	  value_(object->value("value")),
	  description_(object->value("description")),
	  optional_(object->value("optional")),
	  testIncludes_(object->value("include-test")),
	  testLibraries_(object->value("link-test"))
{
	if (name_ == "" && libraries_->size() == 1)
		name_ = libraries_->at(0);
	if (name_ == "") name_ = hex(uint64_t(this));
	BuildParameters::read(object, plan);
}

} // namespace fluxmake
