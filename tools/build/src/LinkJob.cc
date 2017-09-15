/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include "BuildPlan.h"
#include "LinkJob.h"

namespace ccbuild {

LinkJob::LinkJob(const BuildPlan *plan):
    Job(plan->toolChain()->linkCommand(plan)),
    plan_(plan)
{}

bool LinkJob::run()
{
    if (!Job::run()) return false;
    File::save(plan_->previousLinkCommandPath(), command());
    return plan_->toolChain()->createSymlinks(plan_);
}

} // namespace ccbuild
