/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include "DependencyCache.h"
#include "BuildPlan.h"
#include "JobScheduler.h"
#include "AnalyseStage.h"

namespace ccbuild {

bool AnalyseStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan()->goForBuild()) return success_ = true;

    if ( (plan()->options() & BuildPlan::Test) &&
         !(plan()->options() & BuildPlan::BuildTests) ) return success_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan *prerequisite: plan()->prerequisites()) {
        if (!prerequisite->analyseStage()->run())
            return success_ = false;
    }

    if (plan()->options() & BuildPlan::Package) return success_ = true;

    shell()->mkdir(plan()->modulePath());

    Ref<JobScheduler> scheduler;

    Ref<DependencyCache> dependencyCache = DependencyCache::create(plan());
    previousSources_ = dependencyCache->previousSources();
    cacheTime_ = dependencyCache->cacheTime();

    for (string source: plan()->sources()) {
        Ref<Module> module;
        if (dependencyCache->lookup(source, &module)) {
            plan()->modules()->append(module);
        }
        else {
            if (!scheduler) {
                scheduler = createScheduler();
                scheduler->start();
            }
            scheduler->schedule(toolChain()->createAnalyseJob(plan(), source));
        }
    }

    if (!scheduler) return success_ = true;

    for (Ref<Job> job; scheduler->collect(&job);) {
        if (plan()->options() & BuildPlan::Verbose || job->status() != 0)
            fout() << "# " << shell()->beautify(job->command()) << nl;
        if (job->status() != 0) {
            ferr() << job->outputText();
            status_ = job->status();
            return success_ = false;
        }
        Ref<Module> module = toolChain()->finishAnalyseJob(plan(), job);
        dependencyCache->insert(module->sourcePath(), module);
        plan()->modules()->append(module);
    }

    struct ModulePathOrder {
        inline static bool below(Module *a, Module *b) { return a->modulePath() < b->modulePath(); }
    };
    plan()->setModules(plan()->modules()->sort<ModulePathOrder>());

    return success_ = true;
}

} // namespace ccbuild
