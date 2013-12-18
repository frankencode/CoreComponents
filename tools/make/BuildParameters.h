/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXMAKE_BUILDPARAMETERS_H
#define FLUXMAKE_BUILDPARAMETERS_H

#include <flux/Yason.h>

namespace fluxmake
{

using namespace flux;

class BuildPlan;

class BuildParameters: public Object
{
public:
	inline String compiler() const { return compiler_; }
	inline String optimize() const { return optimize_; }
	inline bool linkStatic() const { return linkStatic_; }

	inline StringList *includePaths() const { return includePaths_; }
	inline StringList *libraryPaths() const { return libraryPaths_; }
	inline StringList *libraries() const { return libraries_; }

	inline StringList *customCompileFlags() const { return customCompileFlags_; }
	inline StringList *customLinkFlags() const { return customLinkFlags_; }

protected:
	void read(BuildPlan *plan, YasonObject *object);
	void readSpecific(YasonObject *object);

	String compiler_;
	String optimize_;
	bool linkStatic_;

	Ref<StringList> includePaths_;
	Ref<StringList> libraryPaths_;
	Ref<StringList> libraries_;

	Ref<StringList> customCompileFlags_;
	Ref<StringList> customLinkFlags_;
};

} // namespace fluxmake

#endif // FLUXMAKE_BUILDPARAMETERS_H
