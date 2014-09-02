/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio.h>
#include <flux/Dir.h>
#include <flux/File.h>
#include "BuildPlan.h"
#include "ConfigureStage.h"

namespace fluxmake
{

bool ConfigureStage::run()
{
	if (complete_) return success_;
	complete_ = true;

	for (int i = 0; i < plan()->prerequisites()->count(); ++i) {
		if (!plan()->prerequisites()->at(i)->configureStage()->run())
			return success_ = false;
	}

	if (!plan()->systemPrerequisitesByName()) return success_ = true;

	for (int i = 0; i < plan()->systemPrerequisitesByName()->count(); ++i)
	{
		String name = plan()->systemPrerequisitesByName()->keyAt(i);
		SystemPrerequisiteList *prerequisiteList = plan()->systemPrerequisitesByName()->valueAt(i);

		for (int j = 0; j < prerequisiteList->count(); ++j)
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

			plan()->customCompileFlags()->appendList(prerequisite->customCompileFlags());
			plan()->customLinkFlags()->appendList(prerequisite->customLinkFlags());
			plan()->libraries()->appendList(prerequisite->libraries());
		}
	}

	return success_ = true;
}

bool ConfigureStage::findIncludePath(SystemPrerequisite *prerequisite, String *includePath)
{
	for (int i = 0; i < prerequisite->includePaths()->count(); ++i) {
		String path = prerequisite->includePaths()->at(i);
		if (!path->isAbsolutePath()) path = "/usr/include/" + path;
		if (!Dir::exists(path)) continue;
		int j = 0;
		for (; j < prerequisite->testIncludes()->count(); ++j) {
			String checkPath = path + "/" + prerequisite->testIncludes()->at(j);
			if (!File::exists(checkPath)) break;
		}
		if (j == prerequisite->testIncludes()->count()) {
			*includePath = path;
			return true;
		}
	}
	*includePath = "";
	return prerequisite->includePaths()->count() == 0;
}

bool ConfigureStage::findLibraryPath(SystemPrerequisite *prerequisite, String *libraryPath)
{
	for (int i = 0; i < prerequisite->libraryPaths()->count(); ++i) {
		String path = prerequisite->libraryPaths()->at(i);
		if (!Dir::exists(path)) continue;
		if (plan()->toolChain()->linkTest(plan(), path, prerequisite->testLibraries())) {
			*libraryPath = path;
			return true;
		}
	}
	*libraryPath = "";
	return prerequisite->libraryPaths()->count() == 0;
}

} // namespace fluxmake
