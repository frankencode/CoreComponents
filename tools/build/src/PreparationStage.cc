/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/FileStatus>
#include <cc/glob/Glob>
#include "BuildPlan.h"
#include "JobScheduler.h"
#include "PreparationStage.h"

namespace ccbuild {

using namespace cc::glob;

bool PreparationStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (plan()->options() & BuildPlan::Test) {
        if (!(plan()->options() & BuildPlan::BuildTests))
            return success_ = true;
    }

    if (outOfScope()) return success_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan *prerequisite: plan()->prerequisites()) {
        if (!prerequisite->preparationStage()->run())
            return success_ = false;
    }

    if (!plan()->predicates()) return success_ = true;

    for (Predicate *predicate: plan()->predicates())
    {
        Ref<JobScheduler> scheduler;

        if (predicate->source()->count() == 0) {
            string targetPath = plan()->sourcePath(predicate->target()->replace("%", ""));
            if (plan()->options() & BuildPlan::Clean) {
                if (predicate->clean() != "") {
                    string command = expand(predicate->clean(), "", targetPath);
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
                continue;
            }
            if (predicate->create() != "") {
                if (!shell()->fileStatus(targetPath)->isValid()) {
                    string command = expand(predicate->create(), "", targetPath);
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

        for (string sourceText: predicate->source()) {
            string sourceExpression =
                plan()->sourcePath(
                    sourceText->replace("%", "(?@*)")
                );
            Pattern sourcePattern = sourceExpression;
            Ref<Glob> glob = Glob::open(sourceExpression);
            for (string sourcePath; glob->read(&sourcePath);) {
                string name;
                if (sourceText->contains('%')) {
                    Ref<SyntaxState> state = sourcePattern->match(sourcePath);
                    name = sourcePath->copyRange(state->capture());
                }
                else {
                    name = sourcePath->baseName();
                }
                string targetPath =
                    plan()->sourcePath(
                        predicate->target()->replace("%", name)
                    );
                if (plan()->options() & BuildPlan::Clean) {
                    string command = expand(predicate->clean(), sourcePath, targetPath);
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
                    continue;
                }
                if (
                    (plan()->options() & BuildPlan::Blindfold) ||
                    shell()->fileStatus(targetPath)->lastModified() < shell()->fileStatus(sourcePath)->lastModified()
                ) {
                    string command = expand(predicate->update(), sourcePath, targetPath);
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
            string targetExpression =
                plan()->sourcePath(
                    predicate->target()->replace("%", "(?@*)")
                );
            Pattern targetPattern = targetExpression;
            Ref<Glob> glob = Glob::open(targetExpression);
            for (string targetPath; glob->read(&targetPath);) {
                string name;
                if (predicate->target()->contains('%')) {
                    Ref<SyntaxState> state = targetPattern->match(targetPath);
                    name = targetPath->copyRange(state->capture());
                }
                else {
                    name = targetPath->baseName();
                }
                bool sourceFound = false;
                for (string sourceText: predicate->source()) {
                    string sourcePath =
                        plan()->sourcePath(
                            sourceText->replace("%", name)
                        );
                    if (shell()->fileStatus(sourcePath)->isValid()) {
                        sourceFound = true;
                        break;
                    }
                }
                if (!sourceFound) {
                    string command = expand(predicate->remove(), "", targetPath);
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

string PreparationStage::expand(const string &command, const string &sourcePath, const string &targetPath)
{
    return command
        ->replace("$<", sourcePath)
        ->replace("$@", targetPath);
}

} // namespace ccbuild
