/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/PreparationStage>
#include <cc/build/JobScheduler>
#include <cc/build/BuildStageGuard>
#include <cc/build/BuildShell>
#include <cc/FileInfo>
#include <cc/Glob>
#include <cc/Pattern>
#include <cc/stdio>

namespace cc::build {

bool PreparationStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (plan().options() & BuildOption::Test) {
        if (!(plan().options() & BuildOption::BuildTests))
            return success_ = true;
    }

    if (outOfScope()) return success_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.preparationStage().run())
            return success_ = false;
    }

    if (plan().predicates().count() == 0) return success_ = true;

    shell().mkdir(plan().prestagePath());

    for (const Predicate &predicate: plan().predicates())
    {
        JobScheduler scheduler;

        if (predicate.source().count() == 0) {
            String targetPath = plan().prestagePath(predicate.target().replaced("%", ""));
            if (plan().options() & BuildOption::Clean) {
                if (predicate.clean() != "") {
                    String command = expandCommand(predicate.clean(), "", targetPath);
                    if (plan().options() & BuildOption::Simulate) {
                        fout() << shell().beautify(command) << nl;
                    }
                    else {
                        if (!scheduler) {
                            scheduler = createScheduler();
                            scheduler.start();
                        }
                        scheduler.schedule(Job{command});
                    }
                }
                continue;
            }
            if (predicate.create() != "") {
                if (!shell().fileStatus(targetPath)) {
                    String command = expandCommand(predicate.create(), "", targetPath);
                    if (plan().options() & BuildOption::Simulate) {
                        fout() << shell().beautify(command) << nl;
                    }
                    else {
                        if (!scheduler) {
                            scheduler = createScheduler();
                            scheduler.start();
                        }
                        scheduler.schedule(Job{command});
                    }
                }
            }
        }

        for (const String &sourceText: predicate.source())
        {
            String sourceExpression =
                plan().sourcePath(
                    sourceText.replaced("%", "(?@*)")
                );
            Pattern sourcePattern { sourceExpression };
            Glob glob{sourceExpression};
            for (const String &sourcePath: glob) {
                String name;
                if (sourceText.contains('%')) {
                    List<Range> captures;
                    if (sourcePattern.match(sourcePath, &captures)) {
                        if (captures.count() > 0) {
                            name = sourcePath.copy(captures.at(0));
                        }
                    }
                }
                else {
                    name = sourcePath.baseName();
                }
                String targetPath =
                    plan().prestagePath(
                        predicate.target().replaced("%", name)
                    );
                if (plan().options() & BuildOption::Clean) {
                    String command = expandCommand(predicate.clean(), sourcePath, targetPath);
                    if (plan().options() & BuildOption::Simulate) {
                        fout() << shell().beautify(command) << nl;
                    }
                    else {
                        if (!scheduler) {
                            scheduler = createScheduler();
                            scheduler.start();
                        }
                        scheduler.schedule(Job{command});
                    }
                    continue;
                }
                if (
                    (plan().options() & BuildOption::Blindfold) ||
                    shell().fileStatus(targetPath).lastModified() < shell().fileStatus(sourcePath).lastModified()
                ) {
                    String command = expandCommand(predicate.update(), sourcePath, targetPath);
                    if (plan().options() & BuildOption::Simulate) {
                        fout() << shell().beautify(command) << nl;
                    }
                    else {
                        if (!scheduler) {
                            scheduler = createScheduler();
                            scheduler.start();
                        }
                        scheduler.schedule(Job{command});
                    }
                }
            }
        }

        if (predicate.remove() != "") {
            String targetExpression =
                plan().sourcePath(
                    predicate.target().replaced("%", "(?@*)")
                );
            Pattern targetPattern { targetExpression };
            Glob glob{targetExpression};
            for (const String &targetPath: glob) {
                String name;
                if (predicate.target().contains('%')) {
                    List<Range> captures;
                    if (targetPattern.match(targetPath, &captures)) {
                        if (captures.count() > 0) {
                            name = targetPath.copy(captures.at(0));
                        }
                    }
                }
                else {
                    name = targetPath.baseName();
                }
                bool sourceFound = false;
                for (const String &sourceText: predicate.source()) {
                    String sourcePath =
                        plan().sourcePath(
                            sourceText.replaced("%", name)
                        );
                    if (shell().fileStatus(sourcePath)) {
                        sourceFound = true;
                        break;
                    }
                }
                if (!sourceFound) {
                    String command = expandCommand(predicate.remove(), "", targetPath);
                    if (plan().options() & BuildOption::Simulate) {
                        fout() << shell().beautify(command) << nl;
                    }
                    else {
                        if (!scheduler) {
                            scheduler = createScheduler();
                            scheduler.start();
                        }
                        scheduler.schedule(Job{command});
                    }
                }
            }
        }

        if (scheduler) {
            for (Job job; scheduler.collect(&job);) {
                fout() << shell().beautify(job.command()) << nl;
                if (job.status() != 0) {
                    ferr() << job.outputText();
                    status_ = job.status();
                    return success_ = false;
                }
            }
        }
    }

    return true;
}

String PreparationStage::expandCommand(const String &command, const String &sourcePath, const String &targetPath) const
{
    String s = command;
    s.replace("$CFLAGS", plan().customCompileFlags().join(' '));
    s.replace("$@@", plan().prestagePath());
    s.replace("$<<", plan().projectPath());
    s.replace("$<", sourcePath);
    s.replace("$@", targetPath);
    return s;
}

} // namespace cc::build
