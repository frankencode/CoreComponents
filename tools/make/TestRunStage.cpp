/*
 * Copyright (C) 2007-2014 Frank Mertens.
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

bool TestRunStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    bool cascade = plan()->recipe()->value("test-cascade");

    for (int i = 0; i < plan()->prerequisites()->count(); ++i) {
        BuildPlan *prequisite = plan()->prerequisites()->at(i);
        if (!cascade) {
            String prefix = prequisite->projectPath()->copy(0, plan()->projectPath()->count());
            if (prefix != plan()->projectPath())
                continue;
        }

        TestRunStage *stage = prequisite->testRunStage();
        if (!stage->run()) {
            status_ = stage->status();
            return success_ = false;
        }
    }

    if (!(plan()->options() & BuildPlan::Tests)) return success_ = true;

    Ref<JobScheduler> scheduler = createScheduler();

    for (int i = 0; i < plan()->modules()->count(); ++i) {
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

} // namespace fluxmake
