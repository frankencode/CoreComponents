/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/CompileLinkStage>
#include <cc/build/JobScheduler>
#include <cc/build/BuildStageGuard>
#include <cc/build/BuildShell>
#include <cc/build/ToolChain>
#include <cc/build/InsightDatabase>
#include <cc/File>
#include <cc/stdio>

namespace cc::build {

bool CompileLinkStage::run()
{
    BuildStageGuard guard{this};

    JobScheduler scheduler = createScheduler();
    scheduleJobs(scheduler);

    for (Job job; scheduler.collect(&job);) {
        fout() << shell().beautify(job.command()) << nl;
        ferr() << job.outputText();
        if (job.status() == 0) {
            if (!job.finish()) {
                status_ = 1;
                return success_ = false;
            }
        }
        else {
            status_ = job.status();
            return success_ = false;
        }
    }

    return success_ = true;
}

void CompileLinkStage::scheduleJobs(JobScheduler &scheduler)
{
    if (complete_) return;
    complete_ = true;

    if (!plan().goForBuild()) return;
    if (plan().isSystemSource()) return;

    if (plan().options() & BuildOption::Test) {
        if (!(plan().options() & BuildOption::BuildTests))
            return;
    }

    if (outOfScope()) return;

    for (BuildPlan &prerequisite: plan().prerequisites())
        prerequisite.compileLinkStage().scheduleJobs(scheduler);

    if (plan().options() & BuildOption::Package) return;

    Job linkJob;
    if (!(plan().options() & BuildOption::Tools))
        linkJob = toolChain().createLinkJob(plan());

    InsightDatabase insightDatabase;
    if (plan().options() & BuildOption::Insight) {
        insightDatabase = InsightDatabase{plan().projectPath()};
    }

    bool dirty = false;

    for (const Module &module: plan().modules())
    {
        bool moduleDirty = module.dirty();

        if (plan().options() & BuildOption::Tools) {
            moduleDirty = moduleDirty || !shell().fileStatus(toolChain().linkName(module));
        }

        if (moduleDirty)
        {
            dirty = true;

            Job job;
            if (plan().options() & BuildOption::Tools) {
                job = toolChain().createCompileLinkJob(plan(), module);
                if (!(plan().options() & BuildOption::Simulate))
                    plan().registerLinkDerivative(job);
            }
            else {
                job = toolChain().createCompileJob(plan(), module);
                job.registerDerivative(linkJob);
            }

            if (plan().options() & BuildOption::Simulate) {
                fout() << plan().shell().beautify(job.command()) << ((plan().concurrency() == 1) ? "\n" : " &\n");
            }
            else {
                scheduler.schedule(job);
            }

            if (insightDatabase) {
                insightDatabase.insert(module.sourcePath(), job.command(), module.modulePath());
            }
        }
        else if (insightDatabase) {
            insightDatabase.touch(module.modulePath());
        }
    }

    if (insightDatabase) {
        insightDatabase.sync();

        if (plan().options() & BuildOption::Verbose) {
            ferr() << "Updated insight database " << insightDatabase.path() << nl;
        }
    }

    if (
        plan().modules().count() > 0 &&
        (plan().options() & BuildOption::Simulate) &&
        plan().concurrency() != 1
    )
        fout() << "wait" << nl;

    if (plan().options() & BuildOption::Tools) return;

    if (!dirty)
    {
        FileStatus productStatus = shell().fileStatus(toolChain().linkName(plan()));
        if (!productStatus) dirty = true;

        else {
            String previousLinkCommandPath = plan().previousLinkCommandPath();
            String previousLinkCommand = File::load(previousLinkCommandPath);
            String newLinkCommand = linkJob.command();

            if (newLinkCommand != previousLinkCommand) dirty = true;
            else {
                double productTime = productStatus.lastModified();
                for (const Module &module: plan().modules()) {
                    FileStatus moduleStatus = shell().fileStatus(module.modulePath());
                    if (moduleStatus.lastModified() > productTime) {
                        dirty = true;
                        break;
                    }
                }
                if (!dirty) {
                    FileStatus recipeStatus = shell().fileStatus(plan().recipePath());
                    if (recipeStatus) {
                        if (recipeStatus.lastModified() > productTime)
                            dirty = true;
                    }
                }
            }
        }
    }

    if (!dirty) return;

    if (plan().options() & BuildOption::Simulate) {
        fout() << plan().shell().beautify(linkJob.command()) << nl;
        plan().toolChain().createSymlinks(plan());
    }
    else {
        plan().registerLinkDerivative(linkJob);
        if (plan().options() & BuildOption::Library)
            plan().setLibraryLinkJob(linkJob);

        scheduler.schedule(linkJob);
    }
}

} // namespace cc::build
