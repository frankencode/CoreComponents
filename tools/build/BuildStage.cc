/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/BuildStage>
#include <cc/build/BuildPlan>
#include <cc/build/ConfigureShell>
#include <cc/build/ToolChain>
#include <cc/build/JobScheduler>

namespace cc::build {

const BuildShell &BuildStage::shell() const
{
    return plan().shell();
}

const ToolChain &BuildStage::toolChain() const
{
    return plan().toolChain();
}

JobScheduler BuildStage::createScheduler() const
{
    return JobScheduler{plan().concurrency()};
}

bool BuildStage::outOfScope() const
{
    return
        (plan().options() & (BuildOption::Application|BuildOption::Tools)) &&
        (!plan().projectPath().startsWith(plan().scope()));
}

} // namespace cc::build
