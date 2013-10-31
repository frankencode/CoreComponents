/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FMAKE_SYSTEMPREREQUISITE_H
#define FMAKE_SYSTEMPREREQUISITE_H

#include <fkit/Yason.h>

namespace fmake
{

using namespace fkit;

class SystemPrerequisite: public Object
{
public:
	static Ref<SystemPrerequisite> read(YasonObject *object);

	inline String name() const { return name_; }
	inline String value() const { return value_; }
	inline String description() const { return description_; }
	inline bool optional() const { return optional_; }
	inline StringList *includePath() const { return includePath_; }
	inline StringList *includeTest() const { return includeTest_; }
	inline StringList *linkPath() const { return linkPath_; }
	inline StringList *linkTest() const { return linkTest_; }
	inline StringList *link() const { return link_; }
	inline StringList *compileFlags() const { return compileFlags_; }
	inline StringList *debugCompileFlags() const { return debugCompileFlags_; }
	inline StringList *releaseCompileFlags() const { return releaseCompileFlags_; }

private:
	SystemPrerequisite(YasonObject *object);

	String name_;
	String value_;
	String description_;
	bool optional_;
	Ref<StringList> includePath_;
	Ref<StringList> includeTest_;
	Ref<StringList> linkPath_;
	Ref<StringList> linkTest_;
	Ref<StringList> link_;
	Ref<StringList> compileFlags_;
	Ref<StringList> debugCompileFlags_;
	Ref<StringList> releaseCompileFlags_;
};

typedef List< Ref<SystemPrerequisite> > SystemPrerequisiteList;

} // namespace fmake

#endif // FMAKE_SYSTEMPREREQUISITE_H
