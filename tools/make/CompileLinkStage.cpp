/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio.h>
#include <flux/FileStatus.h>
#include "JobScheduler.h"
#include "BuildShell.h"
#include "BuildPlan.h"
#include "CompileLinkStage.h"

namespace fluxmake
{

bool CompileLinkStage::run()
{
	if (complete_) return success_;
	complete_ = true;

	if ( (plan()->options() & BuildPlan::Tests) &&
	     !(plan()->options() & BuildPlan::BuildTests) ) return success_ = true;

	for (int i = 0; i < plan()->prerequisites()->count(); ++i)
		if (!plan()->prerequisites()->at(i)->compileLinkStage()->run()) return success_ = false;

	if (plan()->options() & BuildPlan::Package) return success_ = true;

	Ref<JobScheduler> compileScheduler;
	Ref<JobScheduler> linkScheduler;

	for (int i = 0; i < plan()->modules()->count(); ++i) {
		Module *module = plan()->modules()->at(i);
		bool dirty = module->dirty();
		if (plan()->options() & BuildPlan::Tools)
			dirty = dirty || !shell()->fileStatus(module->toolName())->exists();
		if (dirty) {
			Ref<Job> job = toolChain()->createCompileJob(plan(), module);
			Ref<Job> linkJob;
			if (plan()->options() & BuildPlan::Tools) linkJob = toolChain()->createLinkJob(plan(), module);
			if (plan()->options() & BuildPlan::Simulate) {
				fout() << shell()->beautify(job->command()) << nl;
				if (linkJob)
					fout() << shell()->beautify(linkJob->command()) << nl;
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
			fout() << shell()->beautify(job->command()) << nl;
			ferr() << job->outputText();
			if (job->status() != 0) {
				status_ = job->status();
				return success_ = false;
			}
		}
	}

	if (linkScheduler) {
		linkScheduler->start();
		for (Ref<Job> job; linkScheduler->collect(&job);) {
			fout() << shell()->beautify(job->command()) << nl;
			ferr() << job->outputText();
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
		for (int i = 0; i < plan()->modules()->count(); ++i) {
			Module *module = plan()->modules()->at(i);
			Ref<FileStatus> moduleStatus = shell()->fileStatus(module->modulePath());
			if (moduleStatus->lastModified() > productTime) {
				dirty = true;
				break;
			}
		}
		Ref<FileStatus> recipeStatus = shell()->fileStatus(plan()->recipePath());
		if (recipeStatus->exists()) {
			if (recipeStatus->lastModified() > productTime) dirty = true;
			for (int i = 0; i < plan()->prerequisites()->count(); ++i) {
				Ref<FileStatus> recipeStatus = shell()->fileStatus(plan()->prerequisites()->at(i)->recipePath());
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

} // namespace fluxmake
