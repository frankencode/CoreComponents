/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/Process>
#include "BuildPlan.h"
#include "JobScheduler.h"
#include "TestRunStage.h"

namespace ccbuild {

TestRunStage::TestRunStage(BuildPlan *plan):
    BuildStage(plan)
{}

bool TestRunStage::run()
{
    bool report = plan()->recipe()->value("test-report");
    if (report) Process::setEnv("TEST_REPORT", "1");

    Ref<JobScheduler> scheduler = JobScheduler::create(plan()->testRunConcurrency());
    scheduleJobs(scheduler);
    int testFailed = 0;

    for (Ref<Job> job; scheduler->collect(&job);) {
        fout() << job->outputText();
        if (job->status() != 0) {
            ++testFailed;
            if (!report) {
                status_ = job->status();
                return success_ = false;
            }
        }
    }

    if (report) status_ = testFailed;
    return success_ = (testFailed == 0);
}

void TestRunStage::scheduleJobs(JobScheduler *scheduler)
{
    if (complete_) return;
    complete_ = true;

    if (!plan()->goForBuild()) return;

    if (outOfScope()) return;

    for (BuildPlan *prerequisite: plan()->prerequisites())
        prerequisite->testRunStage()->scheduleJobs(scheduler);

    if (!(plan()->options() & BuildPlan::Test)) return;

    if (plan()->options() & BuildPlan::Tools) {
        for (Module *module: plan()->modules()) {
            String command = "./" + module->toolName();
            if (plan()->testArgs() != "") command += " " + plan()->testArgs();
            scheduler->schedule(Job::create(command));
        }
    }
    else {
        String command = toolChain()->linkName(plan());
        if (plan()->testArgs() != "") command += " " + plan()->testArgs();
        scheduler->schedule(Job::create(command));
    }
}

} // namespace ccbuild
