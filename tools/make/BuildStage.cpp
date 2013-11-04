/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "JobScheduler.h"
#include "BuildPlan.h"
#include "BuildStage.h"

namespace fmake
{

BuildStage::BuildStage(BuildPlan *plan)
	: plan_(plan),
	  complete_(false),
	  success_(true),
	  status_(0)
{}

BuildShell *BuildStage::shell() const { return plan_->shell(); }
ToolChain *BuildStage::toolChain() const { return plan_->toolChain(); }
Ref<JobScheduler> BuildStage::createScheduler() const { return JobScheduler::create(plan_->concurrency()); }

} // namespace fmake
