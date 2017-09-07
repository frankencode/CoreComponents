/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "JobScheduler.h"
#include "BuildPlan.h"
#include "BuildStage.h"

namespace ccbuild {

BuildStage::BuildStage(BuildPlan *plan):
    plan_(plan),
    complete_(false),
    success_(true),
    status_(0)
{}

BuildShell *BuildStage::shell() const { return plan_->shell(); }
ToolChain *BuildStage::toolChain() const { return plan_->toolChain(); }

Ref<JobScheduler> BuildStage::createScheduler() const
{
    return JobScheduler::create(plan_->concurrency());
}

bool BuildStage::outOfScope() const
{
    if (plan()->options() & (BuildPlan::Application|BuildPlan::Tools)) {
        String prefix = plan()->projectPath()->copy(0, plan()->scope()->count());
        if (prefix != plan()->scope()) return true;
    }
    return false;
}

} // namespace ccbuild
