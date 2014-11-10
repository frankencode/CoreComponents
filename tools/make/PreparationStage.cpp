/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Glob>
#include <flux/FileStatus>
#include <flux/stdio>
#include "BuildPlan.h"
#include "JobScheduler.h"
#include "PreparationStage.h"

namespace fluxmake {

bool PreparationStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    for (int i = 0; i < plan()->prerequisites()->count(); ++i)
        if (!plan()->prerequisites()->at(i)->preparationStage()->run()) return success_ = false;

    if (!plan()->predicates()) return success_ = true;

    Ref<JobScheduler> scheduler;

    for (int i = 0; i < plan()->predicates()->count(); ++i) {
        Predicate *predicate = plan()->predicates()->at(i);
        for (int j = 0; j < predicate->source()->count(); ++j) {
            String sourceExpression =
                plan()->sourcePath(
                    predicate->source()->at(j)->replace("%", "(?@*)")
                );
            Pattern sourcePattern = sourceExpression;
            Ref<Glob> glob = Glob::open(sourceExpression);
            for (String sourcePath; glob->read(&sourcePath);) {
                String name = sourcePath->baseName();
                if (predicate->source()->at(j)->contains('%')) {
                    Ref<SyntaxState> state = sourcePattern->match(sourcePath);
                    name = sourcePath->copy(state->capture());
                }
                else {
                    name = sourcePath->baseName();
                }
                String targetPath = plan()->sourcePath(predicate->target()->replace("%", name));
                if (FileStatus::read(targetPath)->lastModified() < FileStatus::read(sourcePath)->lastModified()) {
                    String command = predicate->command();
                    command = command->replace("$<", sourcePath);
                    command = command->replace("$@", targetPath);
                    command = command->replace("$SOURCE", sourcePath);
                    command = command->replace("$TARGET", targetPath);
                    if (!scheduler) {
                        scheduler = createScheduler();
                        scheduler->start();
                    }
                    scheduler->schedule(Job::create(command));
                }
            }
        }
    }

    if (!scheduler) return success_ = true;

    for (Ref<Job> job; scheduler->collect(&job);) {
        if (plan()->options() & BuildPlan::Verbose || job->status() != 0)
            fout() << "# " << shell()->beautify(job->command()) << nl;
        if (job->status() != 0) {
            ferr() << "# " << job->outputText();
            status_ = job->status();
            return success_ = false;
        }
    }

    return success_ = true;
}

} // namespace fluxmake
