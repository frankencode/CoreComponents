/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/FileStatus>
#include <cc/regexp/Glob>
#include "BuildPlan.h"
#include "JobScheduler.h"
#include "PreparationStage.h"

namespace ccbuild {

using namespace cc::regexp;

bool PreparationStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (plan()->options() & BuildPlan::Test) {
        if (!(plan()->options() & BuildPlan::BuildTests))
            return success_ = true;
    }

    if (outOfScope()) return success_ = true;

    for (BuildPlan *prerequisite: plan()->prerequisites()) {
        if (!prerequisite->preparationStage()->run())
            return success_ = false;
    }

    if (!plan()->predicates()) return success_ = true;

    for (Predicate *predicate: plan()->predicates())
    {
        Ref<JobScheduler> scheduler;

        if (predicate->source()->count() == 0) {
            String targetPath = plan()->sourcePath(predicate->target()->replace("%", ""));
            if (!shell()->fileStatus(targetPath)->isValid()) {
                String command = expand(predicate->create(), "", targetPath);
                if (plan()->options() & BuildPlan::Simulate) {
                    fout() << shell()->beautify(command) << nl;
                }
                else {
                    if (!scheduler) {
                        scheduler = createScheduler();
                        scheduler->start();
                    }
                    scheduler->schedule(Job::create(command));
                }
            }
        }

        for (String sourceText: predicate->source()) {
            String sourceExpression =
                plan()->sourcePath(
                    sourceText->replace("%", "(?@*)")
                );
            RegExp sourcePattern = sourceExpression;
            Ref<Glob> glob = Glob::open(sourceExpression);
            for (String sourcePath; glob->read(&sourcePath);) {
                String name;
                if (sourceText->contains('%')) {
                    Ref<SyntaxState> state = sourcePattern->match(sourcePath);
                    name = sourcePath->copyRange(state->capture());
                }
                else {
                    name = sourcePath->baseName();
                }
                String targetPath =
                    plan()->sourcePath(
                        predicate->target()->replace("%", name)
                    );
                if (
                    (plan()->options() & BuildPlan::Blindfold) ||
                    shell()->fileStatus(targetPath)->lastModified() < shell()->fileStatus(sourcePath)->lastModified()
                ) {
                    String command = expand(predicate->update(), sourcePath, targetPath);
                    if (plan()->options() & BuildPlan::Simulate) {
                        fout() << shell()->beautify(command) << nl;
                    }
                    else {
                        if (!scheduler) {
                            scheduler = createScheduler();
                            scheduler->start();
                        }
                        scheduler->schedule(Job::create(command));
                    }
                }
            }
        }

        if (predicate->remove() != "") {
            String targetExpression =
                plan()->sourcePath(
                    predicate->target()->replace("%", "(?@*)")
                );
            RegExp targetPattern = targetExpression;
            Ref<Glob> glob = Glob::open(targetExpression);
            for (String targetPath; glob->read(&targetPath);) {
                String name;
                if (predicate->target()->contains('%')) {
                    Ref<SyntaxState> state = targetPattern->match(targetPath);
                    name = targetPath->copyRange(state->capture());
                }
                else {
                    name = targetPath->baseName();
                }
                bool sourceFound = false;
                for (String sourceText: predicate->source()) {
                    String sourcePath =
                        plan()->sourcePath(
                            sourceText->replace("%", name)
                        );
                    if (shell()->fileStatus(sourcePath)->isValid()) {
                        sourceFound = true;
                        break;
                    }
                }
                if (!sourceFound) {
                    String command = expand(predicate->remove(), "", targetPath);
                    if (plan()->options() & BuildPlan::Simulate) {
                        fout() << shell()->beautify(command) << nl;
                    }
                    else {
                        if (!scheduler) {
                            scheduler = createScheduler();
                            scheduler->start();
                        }
                        scheduler->schedule(Job::create(command));
                    }
                }
            }
        }

        if (scheduler) {
            for (Ref<Job> job; scheduler->collect(&job);) {
                fout() << shell()->beautify(job->command()) << nl;
                if (job->status() != 0) {
                    ferr() << job->outputText();
                    status_ = job->status();
                    return success_ = false;
                }
            }
        }
    }

    return success_ = true;
}

String PreparationStage::expand(String command, String sourcePath, String targetPath)
{
    return command
        ->replace("$<", sourcePath)
        ->replace("$@", targetPath)
        ->replace("$SOURCE", sourcePath)
        ->replace("$TARGET", targetPath);
}

} // namespace ccbuild
