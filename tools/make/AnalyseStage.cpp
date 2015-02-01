/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include "DependencyCache.h"
#include "BuildPlan.h"
#include "JobScheduler.h"
#include "AnalyseStage.h"

namespace fluxmake {

bool AnalyseStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if ( (plan()->options() & BuildPlan::Test) &&
         !(plan()->options() & BuildPlan::BuildTests) ) return success_ = true;

    for (int i = 0; i < plan()->prerequisites()->count(); ++i)
        if (!plan()->prerequisites()->at(i)->analyseStage()->run()) return success_ = false;

    if (plan()->options() & BuildPlan::Package) return success_ = true;

    shell()->mkdir(plan()->modulePath());

    Ref<JobScheduler> scheduler;

    Ref<DependencyCache> dependencyCache = DependencyCache::create(plan());
    previousSources_ = dependencyCache->previousSources();

    for (int i = 0; i < plan()->sources()->count(); ++i) {
        Ref<Module> module;
        if (dependencyCache->lookup(plan()->sources()->at(i), &module)) {
            plan()->modules()->append(module);
        }
        else {
            if (!scheduler) {
                scheduler = createScheduler();
                scheduler->start();
            }
            scheduler->schedule(toolChain()->createAnalyseJob(plan(), plan()->sources()->at(i)));
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

    return success_ = true;
}

} // namespace fluxmake
