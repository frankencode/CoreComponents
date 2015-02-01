/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include "BuildPlan.h"
#include "JobScheduler.h"
#include "TestRunStage.h"

namespace fluxmake {

TestRunStage::TestRunStage(BuildPlan *plan):
    BuildStage(plan),
    testTotal_(0),
    testFailed_(0)
{}

bool TestRunStage::run()
{
    bool report = plan()->recipe()->value("test-report");
    bool ok = runTests(report);
    if (report) status_ = testFailed_;
    return ok;
}

bool TestRunStage::runTests(bool report)
{
    if (complete_) return success_;
    complete_ = true;

    if (outOfScope()) return success_ = true;

    for (int i = 0; i < plan()->prerequisites()->count(); ++i) {
        BuildPlan *prerequisite = plan()->prerequisites()->at(i);
        TestRunStage *stage = prerequisite->testRunStage();
        if (stage->complete()) continue;
        bool ok = stage->runTests(report);
        testTotal_ += stage->testTotal();
        testFailed_ += stage->testFailed();
        if (!ok) {
            if (!report) {
                status_ = stage->status();
                return success_ = false;
            }
        }
    }

    if (!(plan()->options() & BuildPlan::Test)) return success_ = (testFailed_ == 0);

    Ref<JobScheduler> scheduler = createScheduler();
        // FIXME: there should be only a single test scheduler

    if (plan()->options() & BuildPlan::Tools) {
        for (int i = 0; i < plan()->modules()->count(); ++i) {
            Module *module = plan()->modules()->at(i);
            String command = module->toolName();
            if (plan()->testArgs() != "") command += " " + plan()->testArgs();
            scheduler->schedule(Job::create(command));
            ++testTotal_;
        }
    }
    else {
        String command = toolChain()->linkName(plan());
        if (plan()->testArgs() != "") command += " " + plan()->testArgs();
        scheduler->schedule(Job::create(command));
        ++testTotal_;
    }

    for (Ref<Job> job; scheduler->collect(&job);) {
        fout() << job->command() << nl;
        ferr() << job->outputText();
        if (job->status() != 0) {
            ++testFailed_;
            if (!report) {
                status_ = job->status();
                return success_ = false;
            }
        }
    }

    return success_ = (testFailed_ == 0);
}

} // namespace fluxmake
