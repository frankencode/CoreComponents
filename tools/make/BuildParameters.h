/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_BUILDPARAMETERS_H
#define FLUXMAKE_BUILDPARAMETERS_H

#include <flux/MetaObject>

namespace fluxmake
{

using namespace flux;

class BuildPlan;

class BuildParameters: public Object
{
public:
	inline static Ref<BuildParameters> create() { return new BuildParameters; }
	void read(MetaObject *object, BuildPlan *plan = 0);
	void readSpecific(MetaObject *object);
	void readSpecific(BuildParameters *specific);

	inline String compiler() const { return compiler_; }
	inline String optimize() const { return optimize_; }
	inline bool linkStatic() const { return linkStatic_; }

	inline StringList *includePaths() const { return includePaths_; }
	inline StringList *libraryPaths() const { return libraryPaths_; }
	inline StringList *libraries() const { return libraries_; }

	inline StringList *customCompileFlags() const { return customCompileFlags_; }
	inline StringList *customLinkFlags() const { return customLinkFlags_; }

protected:
	BuildParameters() {}

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
