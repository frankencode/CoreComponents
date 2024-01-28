/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/LinkJob>
#include <cc/build/JobState>
#include <cc/File>

namespace cc::build {

struct LinkJob::State: public Job::State
{
    State(const BuildPlan &plan):
        Job::State{plan.toolChain().linkCommand(plan)},
        plan_{plan}
    {}

    bool run() override
    {
        if (!plan().toolChain().createSymlinks(plan())) return false;
        if (plan().options() & BuildOption::Library) plan().toolChain().generatePkgConfig(plan());
        if (!Job::State::run()) return false;
        if (plan().options() & BuildOption::Strip) plan().toolChain().strip(plan());
        File::save(plan().previousLinkCommandPath(), command_);
        return true;
    }

    const BuildPlan &plan() const { return plan_; }

    BuildPlan plan_;
};

LinkJob::LinkJob(const BuildPlan &plan):
    Job{new State{plan}}
{}

} // namespace cc::build
