/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "SystemPrerequisite.h"

namespace fmake
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
	  includePath_(object->value("include-path")),
	  includeTest_(object->value("include-test")),
	  linkPath_(object->value("link-path")),
	  linkTest_(object->value("link-test")),
	  link_(object->value("link")),
	  compileFlags_(object->value("compile-flags")),
	  debugCompileFlags_(object->value("debug-compile-flags")),
	  releaseCompileFlags_(object->value("release-compile-flags"))
{}

} // namespace fmake
