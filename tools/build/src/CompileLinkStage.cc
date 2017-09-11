/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/File>
#include <cc/FileStatus>
#include "JobScheduler.h"
#include "BuildShell.h"
#include "BuildPlan.h"
#include "CompileLinkStage.h"

namespace ccbuild {

bool CompileLinkStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan()->goForBuild()) return success_ = true;
    if (plan()->isSystemSource()) return success_ = true;

    if (plan()->options() & BuildPlan::Test) {
        if (!(plan()->options() & BuildPlan::BuildTests))
            return success_ = true;
    }

    if (outOfScope()) return success_ = true;

    BuildStageGuard guard(this);

    for (BuildPlan *prerequisite: plan()->prerequisites()) {
        if (!prerequisite->compileLinkStage()->run())
            return success_ = false;
    }

    if (plan()->options() & BuildPlan::Package) return success_ = true;

    Ref<JobScheduler> scheduler;

    for (Module *module: plan()->modules())
    {
        bool dirty = module->dirty();
        if (plan()->options() & BuildPlan::Tools)
            dirty = dirty || !shell()->fileStatus(module->toolName())->isValid();

        if (dirty) {
            Ref<Job> job;
            if (plan()->options() & BuildPlan::Tools)
                job = toolChain()->createCompileLinkJob(plan(), module);
            else
                job = toolChain()->createCompileJob(plan(), module);

            if (plan()->options() & BuildPlan::Simulate) {
                fout() << job->command() << ((plan()->concurrency() == 1) ? "\n" : " &\n");
            }
            else {
                if (!scheduler) {
                    scheduler = createScheduler();
                    scheduler->start();
                }
                scheduler->schedule(job);
            }
        }
    }

    if ((plan()->options() & BuildPlan::Simulate) && plan()->modules()->count() > 0)
        fout() << "wait" << nl;

    if (scheduler) {
        for (Ref<Job> job; scheduler->collect(&job);) {
            fout() << shell()->beautify(job->command()) << nl;
            ferr() << job->outputText();
            if (job->status() != 0) {
                status_ = job->status();
                return success_ = false;
            }
        }
    }

    if (plan()->options() & BuildPlan::Tools)
        return success_ = true;

    String previousLinkCommandPath = plan()->modulePath("LinkCommand");
    String previousLinkCommand = File::load(previousLinkCommandPath);
    String newLinkCommand = toolChain()->linkCommand(plan());

    if (newLinkCommand == previousLinkCommand)
    {
        Ref<FileStatus> productStatus = shell()->fileStatus(toolChain()->linkName(plan()));
        if (
            productStatus->isValid() &&
            *plan()->sources() == *plan()->analyseStage()->previousSources()
        ) {
            double productTime = productStatus->lastModified();
            bool dirty = false;
            for (Module *module: plan()->modules()) {
                Ref<FileStatus> moduleStatus = shell()->fileStatus(module->modulePath());
                if (moduleStatus->lastModified() > productTime) {
                    dirty = true;
                    break;
                }
            }
            Ref<FileStatus> recipeStatus = shell()->fileStatus(plan()->recipePath());
            if (recipeStatus->isValid()) {
                if (recipeStatus->lastModified() > productTime) dirty = true;
                for (BuildPlan *prerequisite: plan()->prerequisites()) {
                    Ref<FileStatus> recipeStatus = shell()->fileStatus(prerequisite->recipePath());
                    if (recipeStatus->lastModified() > productTime) {
                        dirty = true;
                        break;
                    }
                }
            }
            if (!dirty) return success_ = true;
        }
    }

    success_ = toolChain()->link(plan());

    if (success_)
        File::save(previousLinkCommandPath, newLinkCommand);

    return success_;
}

} // namespace ccbuild
