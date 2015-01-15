/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "JobScheduler.h"
#include "BuildPlan.h"
#include "BuildStage.h"

namespace fluxmake {

BuildStage::BuildStage(BuildPlan *plan)
    : plan_(plan),
      complete_(false),
      success_(true),
      status_(0)
{}

BuildShell *BuildStage::shell() const { return plan_->shell(); }
ToolChain *BuildStage::toolChain() const { return plan_->toolChain(); }
Ref<JobScheduler> BuildStage::createScheduler() const { return JobScheduler::create(plan_->concurrency()); }

bool BuildStage::outOfScope() const
{
    if (plan()->options() & BuildPlan::Test) {
        if (!(plan()->options() & BuildPlan::BuildTests))
            return true;
        bool cascade = plan()->recipe()->value("test-cascade");
        if (!cascade) {
            String prefix = plan()->projectPath()->copy(0, plan()->testScope()->count());
            if (prefix != plan()->testScope())
                return true;
        }
    }

    return false;
}

} // namespace fluxmake
