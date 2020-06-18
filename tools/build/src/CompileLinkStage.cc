/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "CompileLinkStage.h"
#include "JobScheduler.h"
#include "BuildShell.h"
#include "BuildPlan.h"
#include <cc/stdio>
#include <cc/File>
#include <cc/FileStatus>

namespace ccbuild {

bool CompileLinkStage::run()
{
    BuildStageGuard guard{this};

    Ref<JobScheduler> scheduler = createScheduler();
    scheduleJobs(scheduler);

    for (Ref<Job> job; scheduler->collect(&job);) {
        fout() << shell()->beautify(job->command()) << nl;
        ferr() << job->outputText();
        if (job->status() == 0) {
            if (!job->finish()) {
                status_ = 1;
                return success_ = false;
            }
        }
        else {
            status_ = job->status();
            return success_ = false;
        }
    }

    return success_ = true;
}

void CompileLinkStage::scheduleJobs(JobScheduler *scheduler)
{
    if (complete_) return;
    complete_ = true;

    if (!plan()->goForBuild()) return;
    if (plan()->isSystemSource()) return;

    if (plan()->options() & BuildPlan::Test) {
        if (!(plan()->options() & BuildPlan::BuildTests))
            return;
    }

    if (outOfScope()) return;

    for (BuildPlan *prerequisite: plan()->prerequisites())
        prerequisite->compileLinkStage()->scheduleJobs(scheduler);

    if (plan()->options() & BuildPlan::Package) return;

    Ref<Job> linkJob;
    if (!(plan()->options() & BuildPlan::Tools))
        linkJob = toolChain()->createLinkJob(plan());

    bool dirty = false;

    for (const Module *module: plan()->modules())
    {
        bool moduleDirty = module->dirty();
        if (plan()->options() & BuildPlan::Tools)
            moduleDirty = moduleDirty || !shell()->fileStatus(module->toolName())->isValid();

        if (moduleDirty)
        {
            dirty = true;

            Ref<Job> job;
            if (plan()->options() & BuildPlan::Tools) {
                job = toolChain()->createCompileLinkJob(plan(), module);
                if (!(plan()->options() & BuildPlan::Simulate))
                    plan()->registerLinkDerivative(job);
            }
            else {
                job = toolChain()->createCompileJob(plan(), module);
                job->registerDerivative(linkJob);
            }

            if (plan()->options() & BuildPlan::Simulate)
                fout() << plan()->shell()->beautify(job->command()) << ((plan()->concurrency() == 1) ? "\n" : " &\n");
            else
                scheduler->schedule(job);
        }
    }

    if (
        plan()->modules()->count() > 0 &&
        (plan()->options() & BuildPlan::Simulate) &&
        plan()->concurrency() != 1
    )
        fout() << "wait" << nl;

    if (plan()->options() & BuildPlan::Tools) return;

    if (!dirty)
    {
        FileStatus productStatus = shell()->fileStatus(toolChain()->linkName(plan()));
        if (!productStatus->isValid()) dirty = true;
        else {
            String previousLinkCommandPath = plan()->previousLinkCommandPath();
            String previousLinkCommand = File::load(previousLinkCommandPath);
            String newLinkCommand = linkJob->command();

            if (newLinkCommand != previousLinkCommand) dirty = true;
            else {
                double productTime = productStatus->lastModified();
                for (const Module *module: plan()->modules()) {
                    FileStatus moduleStatus = shell()->fileStatus(module->modulePath());
                    if (moduleStatus->lastModified() > productTime) {
                        dirty = true;
                        break;
                    }
                }
                if (!dirty) {
                    FileStatus recipeStatus = shell()->fileStatus(plan()->recipePath());
                    if (recipeStatus->isValid()) {
                        if (recipeStatus->lastModified() > productTime)
                            dirty = true;
                    }
                }
            }
        }
    }

    if (!dirty) return;

    if (plan()->options() & BuildPlan::Simulate) {
        fout() << plan()->shell()->beautify(linkJob->command()) << nl;
        plan()->toolChain()->createSymlinks(plan());
    }
    else {
        plan()->registerLinkDerivative(linkJob);
        if (plan()->options() & BuildPlan::Library)
            plan()->setLibraryLinkJob(linkJob);

        scheduler->schedule(linkJob);
    }
}

} // namespace ccbuild
