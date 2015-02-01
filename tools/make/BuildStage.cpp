/*
 * Copyright (C) 2007-2015 Frank Mertens.
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
    if (plan()->options() & (BuildPlan::Application|BuildPlan::Tools)) {
        String prefix = plan()->projectPath()->copy(0, plan()->scope()->count());
        if (prefix != plan()->scope()) return true;
    }
    return false;
}

} // namespace fluxmake
