/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "BuildPlan.h"
#include "BuildParameters.h"

namespace fluxmake
{

void BuildParameters::read(BuildPlan *plan, YasonObject *object)
{
	compiler_ = object->value("compiler");
	optimize_ = object->value("optimize");
	linkStatic_ = object->value("static");

	includePaths_ = cast<StringList>(object->value("include-path"));
	libraryPaths_ = cast<StringList>(object->value("link-path"));
	libraries_ = cast<StringList>(object->value("link"));
	if (!includePaths_) includePaths_ = StringList::create();
	if (!libraryPaths_) libraryPaths_ = StringList::create();
	if (!libraries_) libraries_ = StringList::create();

	customCompileFlags_ = cast<StringList>(object->value("compile-flags"));
	customLinkFlags_ = cast<StringList>(object->value("link-flags"));
	if (!customCompileFlags_) customCompileFlags_ = StringList::create();
	if (!customLinkFlags_) customLinkFlags_ = StringList::create();

	if (object->hasChildren()) {
		for (int i = 0; i < object->children()->size(); ++i) {
			YasonObject *child = object->children()->at(i);
			if (child->className() == "Debug") {
				if (plan->options() && BuildPlan::Debug) readSpecific(child);
			}
			else if (child->className() == "Release") {
				if (plan->options() && BuildPlan::Release) readSpecific(child);
			}
		}
	}
}

void BuildParameters::readSpecific(YasonObject *object)
{
	String specificCompiler = object->value("compiler");
	if (specificCompiler != "") compiler_ = specificCompiler;

	String speficicOptimize = object->value("optimize");
	if (speficicOptimize != "") optimize_ = speficicOptimize;

	if (object->value("static")) linkStatic_ = true;

	includePaths_->append(cast<StringList>(object->value("include-path")));
	libraryPaths_->append(cast<StringList>(object->value("link-path")));
	libraries_->append(cast<StringList>(object->value("link")));

	customCompileFlags_->append(cast<StringList>(object->value("compile-flags")));
	customLinkFlags_->append(cast<StringList>(object->value("link-flags")));
}

} // namespace fluxmake
