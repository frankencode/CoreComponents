/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include "JobScheduler.h"
#include "BuildPlan.h"
#include "PreprocessStage.h"

namespace ccbuild {

bool PreprocessStage::run()
{
    BuildStageGuard guard{this};

    Ref<JobScheduler> scheduler = createScheduler();
    scheduleJobs(scheduler);

    for (Ref<Job> job; scheduler->collect(&job);) {
        if (job->status() == 0) {
            fout() << job->outputText();
        }
        else {
            ferr() << job->command() << nl;
            ferr() << job->outputText();
            status_ = job->status();
            return success_ = false;
        }
    }

    return success_ = true;
}

void PreprocessStage::scheduleJobs(JobScheduler *scheduler)
{
    if (complete_) return;
    complete_ = true;

    if (!plan()->goForBuild()) return;
    if (plan()->isSystemSource()) return;

    if (outOfScope()) return;
    if (plan()->options() & BuildPlan::Test) return;

    for (BuildPlan *prerequisite: plan()->prerequisites())
        prerequisite->preprocessStage()->scheduleJobs(scheduler);

    for (const Module *module: plan()->modules()) {
        scheduler->schedule(
            toolChain()->createPreprocessJob(plan(), module)
        );
    }
}

} // namespace ccbuild
