/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/stdio.h>
#include <fkit/Dir.h>
#include <fkit/File.h>
#include "BuildPlan.h"
#include "ConfigureStage.h"

namespace fmake
{

bool ConfigureStage::run()
{
	if (complete_) return success_;
	complete_ = true;

	for (int i = 0; i < plan()->systemPrerequisitesByName()->size(); ++i)
	{
		String name = plan()->systemPrerequisitesByName()->keyAt(i);
		SystemPrerequisiteList *prerequisiteList = plan()->systemPrerequisitesByName()->valueAt(i);

		for (int j = 0; j < prerequisiteList->size(); ++j)
		{
			SystemPrerequisite *prerequisite = prerequisiteList->at(j);
			String includePath;
			if (!findIncludePath(prerequisite, &includePath)) {
				if (!prerequisite->optional()) {
					ferr() << "Missing system prerequisite \"" << prerequisite->name() << "\": Include path not found" << nl;
					return success_ = false;
				}
			}
			String libraryPath;
			if (!findLibraryPath(prerequisite, &libraryPath)) {
				if (!prerequisite->optional()) {
					ferr() << "Missing system prerequisite \"" << prerequisite->name() << "\": Library path not found" << nl;
					return success_ = false;
				}
			}

			if (includePath != "") plan()->includePaths()->append(includePath);
			if (libraryPath != "") plan()->libraryPaths()->append(libraryPath);

			for (int k = 0; k < prerequisite->libraries()->size(); ++k)
				plan()->libraries()->append(prerequisite->libraries()->at(k));

			for (int k = 0; k < prerequisite->compileFlags()->size(); ++k)
				plan()->customCompileFlags()->append(prerequisite->compileFlags()->at(k));

			if (plan()->options() & BuildPlan::Debug) {
				for (int k = 0; k < prerequisite->debugCompileFlags()->size(); ++k)
					plan()->customCompileFlags()->append(prerequisite->debugCompileFlags()->at(k));
			}
			if (plan()->options() & BuildPlan::Release) {
				for (int k = 0; k < prerequisite->releaseCompileFlags()->size(); ++k)
					plan()->customCompileFlags()->append(prerequisite->releaseCompileFlags()->at(k));
			}
		}
	}

	return success_ = true;
}

bool ConfigureStage::findIncludePath(SystemPrerequisite *prerequisite, String *includePath)
{
	for (int i = 0; i < prerequisite->includePaths()->size(); ++i) {
		String path = prerequisite->includePaths()->at(i);
		if (!Dir::exists(path)) continue;
		int j = 0;
		for (; j < prerequisite->testIncludes()->size(); ++j) {
			String checkPath = path + "/" + prerequisite->testIncludes()->at(j);
			if (!File::exists(checkPath)) break;
		}
		if (j == prerequisite->testIncludes()->size()) {
			*includePath = path;
			return true;
		}
	}
	*includePath = "";
	return prerequisite->includePaths()->size() == 0;
}

bool ConfigureStage::findLibraryPath(SystemPrerequisite *prerequisite, String *libraryPath)
{
	for (int i = 0; i < prerequisite->libraryPaths()->size(); ++i) {
		String path = prerequisite->libraryPaths()->at(i);
		if (!Dir::exists(path)) continue;
		if (plan()->toolChain()->linkTest(plan(), path, prerequisite->testLibraries())) {
			*libraryPath = path;
			return true;
		}
	}
	*libraryPath = "";
	return prerequisite->libraryPaths()->size() == 0;
}

} // namespace fmake
