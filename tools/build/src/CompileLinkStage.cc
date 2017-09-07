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

    for (int i = 0; i < plan()->prerequisites()->count(); ++i) {
        BuildPlan *prerequisite = plan()->prerequisites()->at(i);
        if (!prerequisite->compileLinkStage()->run())
            return success_ = false;
    }

    if (plan()->options() & BuildPlan::Package) return success_ = true;

    Ref<JobScheduler> compileScheduler;
    Ref<JobScheduler> linkScheduler;
    Ref<StringList> compileList;
    Ref<StringList> linkList;

    for (int i = 0; i < plan()->modules()->count(); ++i) {
        Module *module = plan()->modules()->at(i);
        bool dirty = module->dirty();
        if (plan()->options() & BuildPlan::Tools)
            dirty = dirty || !shell()->fileStatus(module->toolName())->isValid();
        if (dirty) {
            Ref<Job> job = toolChain()->createCompileJob(plan(), module);
            Ref<Job> linkJob;
            if (plan()->options() & BuildPlan::Tools) linkJob = toolChain()->createLinkJob(plan(), module);
            if (plan()->options() & BuildPlan::Simulate) {
                if (!compileList) compileList = StringList::create();
                compileList << shell()->beautify(job->command());
                if (linkJob) {
                    if (!linkList) linkList = StringList::create();
                    linkList << shell()->beautify(linkJob->command());
                }
            }
            else {
                if (!compileScheduler) {
                    compileScheduler = createScheduler();
                    compileScheduler->start();
                }
                compileScheduler->schedule(job);
                if (linkJob) {
                    if (!linkScheduler) linkScheduler = createScheduler();
                    linkScheduler->schedule(linkJob);
                }
            }
        }
    }

    if (compileList) {
        for (String command: compileList)
            fout() << command << ((plan()->concurrency() == 1) ? "\n" : " &\n");
        fout() << "wait" << nl;
    }

    if (linkList) {
        for (String command: linkList)
            fout() << command << ((plan()->concurrency() == 1) ? "\n" : " &\n");
        fout() << "wait" << nl;
    }

    if (compileScheduler) {
        for (Ref<Job> job; compileScheduler->collect(&job);) {
            fout() << shell()->beautify(job->command()) << nl;
            ferr() << job->outputText();
            if (job->status() != 0) {
                status_ = job->status();
                return success_ = false;
            }
        }
    }

    if (linkScheduler) {
        linkScheduler->start();
        for (Ref<Job> job; linkScheduler->collect(&job);) {
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
            for (int i = 0; i < plan()->modules()->count(); ++i) {
                Module *module = plan()->modules()->at(i);
                Ref<FileStatus> moduleStatus = shell()->fileStatus(module->modulePath());
                if (moduleStatus->lastModified() > productTime) {
                    dirty = true;
                    break;
                }
            }
            Ref<FileStatus> recipeStatus = shell()->fileStatus(plan()->recipePath());
            if (recipeStatus->isValid()) {
                if (recipeStatus->lastModified() > productTime) dirty = true;
                for (int i = 0; i < plan()->prerequisites()->count(); ++i) {
                    Ref<FileStatus> recipeStatus = shell()->fileStatus(plan()->prerequisites()->at(i)->recipePath());
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
