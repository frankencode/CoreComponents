/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/stdio.h>
#include <fkit/FileStatus.h>
#include "JobScheduler.h"
#include "BuildShell.h"
#include "BuildPlan.h"
#include "CompileLinkStage.h"

namespace fmake
{

bool CompileLinkStage::run()
{
	if (complete_) return success_;
	complete_ = true;

	if ( (plan()->options() & BuildPlan::Tests) &&
	     !(plan()->options() & BuildPlan::BuildTests) ) return success_ = true;

	for (int i = 0; i < plan()->prerequisites()->size(); ++i)
		if (!plan()->prerequisites()->at(i)->compileLinkStage()->run()) return success_ = false;

	if (plan()->options() & BuildPlan::Package) return success_ = true;

	Ref<JobScheduler> compileScheduler;
	Ref<JobScheduler> linkScheduler;

	for (int i = 0; i < plan()->modules()->size(); ++i) {
		Module *module = plan()->modules()->at(i);
		bool dirty = module->dirty();
		if (plan()->options() & BuildPlan::Tools)
			dirty = dirty || !shell()->fileStatus(module->toolName())->exists();
		if (dirty) {
			Ref<Job> job = toolChain()->createCompileJob(plan(), module);
			Ref<Job> linkJob;
			if (plan()->options() & BuildPlan::Tools) linkJob = toolChain()->createLinkJob(plan(), module);
			if (plan()->options() & BuildPlan::Simulate) {
				ferr() << shell()->beautify(job->command()) << nl;
				if (linkJob)
					ferr() << shell()->beautify(linkJob->command()) << nl;
			}
			else {
				if (!compileScheduler) {
					compileScheduler = createScheduler();
					compileScheduler->start();
				}
				compileScheduler->schedule(job);
				if (linkJob) {
					if (!linkScheduler) linkScheduler = createScheduler();
					linkScheduler->schedule(linkJob);
				}
			}
		}
	}

	if (compileScheduler) {
		for (Ref<Job> job; compileScheduler->collect(&job);) {
			ferr() << shell()->beautify(job->command()) << nl;
			fout() << job->outputText();
			if (job->status() != 0) {
				status_ = job->status();
				return success_ = false;
			}
		}
	}

	if (linkScheduler) {
		linkScheduler->start();
		for (Ref<Job> job; linkScheduler->collect(&job);) {
			ferr() << shell()->beautify(job->command()) << nl;
			fout() << job->outputText();
			if (job->status() != 0) {
				status_ = job->status();
				return success_ = false;
			}
		}
	}

	if (plan()->options() & BuildPlan::Tools)
		return success_ = true;

	Ref<FileStatus> productStatus = shell()->fileStatus(toolChain()->linkName(plan()));
	if (productStatus->exists() && *plan()->sources() == *plan()->analyseStage()->previousSources()) {
		double productTime = productStatus->lastModified();
		bool dirty = false;
		for (int i = 0; i < plan()->modules()->size(); ++i) {
			Module *module = plan()->modules()->at(i);
			Ref<FileStatus> moduleStatus = shell()->fileStatus(module->modulePath());
			if (moduleStatus->lastModified() > productTime) {
				dirty = true;
				break;
			}
		}
		Ref<FileStatus> recipeStatus = shell()->fileStatus(plan()->recipe()->path());
		if (recipeStatus->exists()) {
			if (recipeStatus->lastModified() > productTime) dirty = true;
			for (int i = 0; i < plan()->prerequisites()->size(); ++i) {
				Ref<FileStatus> recipeStatus = shell()->fileStatus(plan()->prerequisites()->at(i)->recipe()->path());
				if (recipeStatus->lastModified() > productTime) {
					dirty = true;
					break;
				}
			}
		}
		if (!dirty) return success_ = true;
	}

	return success_ = toolChain()->link(plan());
}

} // namespace fmake
