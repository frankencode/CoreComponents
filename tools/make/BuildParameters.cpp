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

void BuildParameters::read(YasonObject *object, BuildPlan *plan)
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
			else {
				#ifdef __linux
				if (child->className() == "Linux") readSpecific(child);
				#endif
				#ifdef __OpenBSD__
				if (child->className() == "OpenBSD") readSpecific(child);
				#endif
				#ifdef __FreeBSD__
				if (child->className() == "FreeBSD") readSpecific(child);
				#endif
				#ifdef __NetBSD__
				if (child->className() == "NetBSD") readSpecific(child);
				#endif
				#ifdef __DragonFly__
				if (child->className() == "DragonFly") readSpecific(child);
				#endif
				#ifdef __APPLE__
				if (child->className() == "MacOSX") readSpecific(child);
				#endif
				#ifdef __sun
				if (child->className() == "Solaris") readSpecific(child);
				#endif
				#ifdef __MACH__
				if (child->className() == "Mach") readSpecific(child);
				#endif
				#ifdef __hpux
				if (child->className() == "HPUX") readSpecific(child);
				#endif
			}
		}
	}
}

void BuildParameters::readSpecific(YasonObject *object)
{
	Ref<BuildParameters> specific = BuildParameters::create();
	specific->read(object);
	readSpecific(specific);
}

void BuildParameters::readSpecific(BuildParameters *specific)
{
	if (specific->compiler() != "") compiler_ = specific->compiler();
	if (specific->optimize() != "") optimize_ = specific->optimize();
	if (specific->linkStatic()) linkStatic_ = true;

	includePaths_->append(specific->includePaths());
	libraryPaths_->append(specific->libraryPaths());
	libraries_->append(specific->libraries());

	customCompileFlags_->append(specific->customCompileFlags());
	customLinkFlags_->append(specific->customLinkFlags());
}

} // namespace fluxmake
