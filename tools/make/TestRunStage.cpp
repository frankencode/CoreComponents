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

int TestRunStage::run()
{
	if (complete_) return status_;
	complete_ = true;

	for (int i = 0; i < plan()->prerequisites()->size(); ++i) {
		status_ = plan()->prerequisites()->at(i)->testRunStage()->run();
		if (status_ != 0) return status_;
	}

	if (!(plan()->options() & BuildPlan::Tests)) return 0;

	Ref<JobScheduler> scheduler = JobScheduler::create();

	for (int i = 0; i < plan()->modules()->size(); ++i) {
		Module *module = plan()->modules()->at(i);
		scheduler->schedule(toolChain()->createTestJob(plan(), module));
	}

	for (Ref<Job> job; scheduler->collect(&job);) {
		ferr() << job->command() << nl;
		fout() << job->outputText();
		if (job->status() != 0)
			return status_ = job->status();
	}

	return status_ = 0;
}

} // namespace fmake
