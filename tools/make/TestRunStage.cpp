/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/stdio.h>
#include "BuildPlan.h"
#include "JobScheduler.h"
#include "TestRunStage.h"

namespace fmake
{

bool TestRunStage::run()
{
	if (complete_) return success_;
	complete_ = true;

	for (int i = 0; i < plan()->prerequisites()->size(); ++i) {
		TestRunStage *stage = plan()->prerequisites()->at(i)->testRunStage();
		if (!stage->run()) {
			status_ = stage->status();
			return success_ = false;
		}
	}

	if (!(plan()->options() & BuildPlan::Tests)) return success_ = true;

	Ref<JobScheduler> scheduler = createScheduler();

	for (int i = 0; i < plan()->modules()->size(); ++i) {
		Module *module = plan()->modules()->at(i);
		scheduler->schedule(toolChain()->createTestJob(plan(), module));
	}

	for (Ref<Job> job; scheduler->collect(&job);) {
		fout() << job->command() << nl;
		ferr() << job->outputText();
		if (job->status() != 0) {
			status_ = job->status();
			return success_ = false;
		}
	}

	return success_ = true;
}

} // namespace fmake
