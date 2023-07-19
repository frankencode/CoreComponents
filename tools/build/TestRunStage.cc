/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/TestRunStage>
#include <cc/build/JobScheduler>
#include <cc/build/BuildStageGuard>
#include <cc/build/ToolChain>
#include <cc/build/ObjectFile>
#include <cc/Process>
#include <cc/stdio>

namespace cc::build {

bool TestRunStage::run()
{
    bool report = plan().testReport();

    if (report) Process::setEnv("TEST_REPORT", "1");

    JobScheduler scheduler = JobScheduler{plan().testRunConcurrency()};
    scheduleJobs(scheduler);
    int testFailed = 0;

    for (Job job; scheduler.collect(&job);) {
        fout() << job.outputText();
        if (job.status() != 0) {
            ++testFailed;
            if (!report) {
                status_ = job.status();
                return success_ = false;
            }
        }
    }

    if (report) status_ = testFailed;
    return success_ = (testFailed == 0);
}

void TestRunStage::scheduleJobs(JobScheduler &scheduler)
{
    if (complete_) return;
    complete_ = true;

    if (!plan().goForBuild()) return;

    if (outOfScope()) return;

    for (BuildPlan &prerequisite: plan().prerequisites())
        prerequisite.testRunStage().scheduleJobs(scheduler);

    if (!(plan().options() & BuildOption::Test)) return;

    if (plan().options() & BuildOption::Tools) {
        for (ObjectFile &objectFile: plan().objectFiles()) {
            String command = "./" + toolChain().linkName(objectFile);
            if (plan().testArgs() != "") command += " " + plan().testArgs();
            scheduler.schedule(Job{command});
        }
    }
    else {
        String command = toolChain().linkName(plan());
        if (plan().testArgs() != "") command += " " + plan().testArgs();
        scheduler.schedule(Job{command});
    }
}

} // namespace cc::build
