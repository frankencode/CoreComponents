/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/build/PreprocessStage>
#include <cc/build/BuildStageGuard>
#include <cc/build/JobScheduler>
#include <cc/build/ToolChain>
#include <cc/stdio>

namespace cc::build {

bool PreprocessStage::run()
{
    BuildStageGuard guard{this};

    JobScheduler scheduler;
    scheduleJobs(scheduler);

    if (scheduler) {
        for (Job job; scheduler.collect(&job);) {
            if (job.status() == 0) {
                fout() << job.outputText();
            }
            else {
                ferr() << job.command() << nl;
                ferr() << job.outputText();
                status_ = job.status();
                return success_ = false;
            }
        }
    }

    return success_ = true;
}

void PreprocessStage::scheduleJobs(JobScheduler &scheduler)
{
    if (complete_) return;
    complete_ = true;

    if (!plan().goForBuild()) return;
    if (plan().isSystemSource()) return;

    if (outOfScope()) return;
    if (plan().options() & BuildOption::Test) return;

    if (!scheduler) scheduler = createScheduler();

    for (BuildPlan &prerequisite: plan().prerequisites())
        prerequisite.preprocessStage().scheduleJobs(scheduler);

    for (const Module &module: plan().modules()) {
        scheduler.schedule(
            toolChain().createPreprocessJob(plan(), module)
        );
    }
}

} // namespace cc::build
