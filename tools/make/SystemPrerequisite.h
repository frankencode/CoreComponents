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

#include <flux/Yason.h>

namespace fmake
{

using namespace flux;

class SystemPrerequisite: public Object
{
public:
	static Ref<SystemPrerequisite> read(YasonObject *object);

	inline String name() const { return name_; }
	inline String value() const { return value_; }
	inline String description() const { return description_; }
	inline bool optional() const { return optional_; }
	inline StringList *includePaths() const { return includePaths_; }
	inline StringList *testIncludes() const { return testIncludes_; }
	inline StringList *libraryPaths() const { return libraryPaths_; }
	inline StringList *testLibraries() const { return testLibraries_; }
	inline StringList *libraries() const { return libraries_; }
	inline StringList *compileFlags() const { return compileFlags_; }
	inline StringList *debugCompileFlags() const { return debugCompileFlags_; }
	inline StringList *releaseCompileFlags() const { return releaseCompileFlags_; }

private:
	SystemPrerequisite(YasonObject *object);

	String name_;
	String value_;
	String description_;
	bool optional_;
	Ref<StringList> includePaths_;
	Ref<StringList> testIncludes_;
	Ref<StringList> libraryPaths_;
	Ref<StringList> testLibraries_;
	Ref<StringList> libraries_;
	Ref<StringList> compileFlags_;
	Ref<StringList> debugCompileFlags_;
	Ref<StringList> releaseCompileFlags_;
};

typedef List< Ref<SystemPrerequisite> > SystemPrerequisiteList;
typedef Map< String, Ref<SystemPrerequisiteList> > SystemPrerequisitesByName;

} // namespace fmake

#endif // FLUXMAKE_SYSTEMPREREQUISITE_H
