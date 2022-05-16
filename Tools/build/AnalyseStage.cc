/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/build/AnalyseStage>
#include <cc/build/JobScheduler>
#include <cc/build/BuildStageGuard>
#include <cc/build/BuildShell>
#include <cc/build/DependencyCache>
#include <cc/stdio>

namespace cc::build {

bool AnalyseStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan().goForBuild()) return success_ = true;

    if ( (plan().options() & BuildOption::Test) &&
         !(plan().options() & BuildOption::BuildTests)) {
        return success_ = true;
    }

    BuildStageGuard guard{this};

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.analyseStage().run())
            return success_ = false;
    }

    if (plan().options() & BuildOption::Package) return success_ = true;

    shell().mkdir(plan().modulePath());

    JobScheduler scheduler;

    DependencyCache dependencyCache { plan() };
    previousSources_ = dependencyCache.previousSources();
    cacheTime_ = dependencyCache.cacheTime();

    for (const String &source: plan().sources()) {
        Module module;
        if (dependencyCache.lookup(source, &module)) {
            plan().modules().append(module);
        }
        else {
            if (!scheduler) {
                scheduler = createScheduler();
                scheduler.start();
            }
            scheduler.schedule(toolChain().createAnalyseJob(plan(), source));
        }
    }

    if (!scheduler) return success_ = true;

    for (Job job; scheduler.collect(&job);) {
        if (plan().options() & BuildOption::Verbose || job.status() != 0)
            fout() << "# " << shell().beautify(job.command()) << nl;
        if (job.status() != 0) {
            ferr() << job.outputText();
            status_ = job.status();
            return success_ = false;
        }

        Module module = toolChain().finishAnalyseJob(plan(), job);
        dependencyCache.insert(module.sourcePath(), module);
        plan().modules().append(module);
    }

    plan().modules().sort(
        [](const Module &a, const Module &b) {
            return a.modulePath() < b.modulePath();
        }
    );

    return success_ = true;
}

} // namespace cc::build
