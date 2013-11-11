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

Ref<SystemPrerequisite> SystemPrerequisite::read(YasonObject *object)
{
	return new SystemPrerequisite(object);
}

SystemPrerequisite::SystemPrerequisite(YasonObject *object)
	: name_(object->value("name")),
	  value_(object->value("value")),
	  description_(object->value("description")),
	  optional_(object->value("optional")),
	  includePaths_(object->value("include-path")),
	  testIncludes_(object->value("include-test")),
	  libraryPaths_(object->value("link-path")),
	  testLibraries_(object->value("link-test")),
	  libraries_(object->value("link")),
	  compileFlags_(object->value("compile-flags")),
	  debugCompileFlags_(object->value("debug-compile-flags")),
	  releaseCompileFlags_(object->value("release-compile-flags"))
{
	if (name_ == "" && libraries_->size() == 1)
		name_ = libraries_->at(0);
	if (name_ == "") name_ = hex(uint64_t(this));
}

} // namespace fluxmake
