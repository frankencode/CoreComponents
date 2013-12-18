/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXMAKE_SYSTEMPREREQUISITE_H
#define FLUXMAKE_SYSTEMPREREQUISITE_H

#include "BuildParameters.h"

namespace fluxmake
{

using namespace flux;

class BuildPlan;

class SystemPrerequisite: public BuildParameters
{
public:
	static Ref<SystemPrerequisite> read(BuildPlan *plan, YasonObject *object);

	inline String name() const { return name_; }
	inline String value() const { return value_; }
	inline String description() const { return description_; }
	inline bool optional() const { return optional_; }

	inline StringList *testIncludes() const { return testIncludes_; }
	inline StringList *testLibraries() const { return testLibraries_; }

private:
	SystemPrerequisite(BuildPlan *plan, YasonObject *object);

	String name_;
	String value_;
	String description_;
	bool optional_;

	Ref<StringList> testIncludes_;
	Ref<StringList> testLibraries_;
};

typedef List< Ref<SystemPrerequisite> > SystemPrerequisiteList;
typedef Map< String, Ref<SystemPrerequisiteList> > SystemPrerequisitesByName;

} // namespace fluxmake

#endif // FLUXMAKE_SYSTEMPREREQUISITE_H
