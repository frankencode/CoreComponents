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

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        prerequisite.compileLinkStage().scheduleJobs(scheduler);
    }

    if (plan().options() & BuildOption::Package) return;

    shell().mkdir(plan().objectsPath());

    gatherObjects();

    Job linkJob;
    if (!(plan().options() & BuildOption::Tools)) {
        linkJob = toolChain().createLinkJob(plan());
    }

    InsightDatabase insightDatabase;
    if (plan().options() & BuildOption::Insight) {
        insightDatabase = InsightDatabase{plan().projectPath()};
    }

    bool dirty = false;

    for (const ObjectFile &objectFile: plan().objectFiles())
    {
        bool objectFileDirty = objectFile.dirty();

        if (plan().options() & BuildOption::Tools) {
            objectFileDirty = objectFileDirty || !shell().fileStatus(toolChain().linkName(objectFile));
        }

        if (objectFileDirty)
        {
            dirty = true;

            Job job;
            if (plan().options() & BuildOption::Tools) {
                job = toolChain().createCompileLinkJob(plan(), objectFile);
                if (!(plan().options() & BuildOption::Simulate)) {
                    plan().registerLinkDerivative(job);
                }
            }
            else {
                job = toolChain().createCompileJob(plan(), objectFile);
                job.registerDerivative(linkJob);
            }

            if (plan().options() & BuildOption::Simulate) {
                fout() << plan().shell().beautify(job.command()) << ((plan().concurrency() == 1) ? "\n" : " &\n");
            }
            else {
                scheduler.schedule(job);
            }

            if (insightDatabase) {
                insightDatabase.insert(objectFile.sourcePath(), job.command(), objectFile.objectFilePath());
            }
        }
        else if (insightDatabase) {
            insightDatabase.touch(objectFile.objectFilePath());
        }
    }

    if (insightDatabase) {
        insightDatabase.sync();

        if (plan().options() & BuildOption::Verbose) {
            ferr() << "Updated insight database " << insightDatabase.path() << nl;
        }
    }

    if (
        plan().objectFiles().count() > 0 &&
        (plan().options() & BuildOption::Simulate) &&
        plan().concurrency() != 1
    ) {
        fout() << "wait" << nl;
    }

    if (plan().options() & BuildOption::Tools) return;

    if (!dirty)
    {
        FileInfo productStatus = shell().fileStatus(toolChain().linkName(plan()));
        if (!productStatus) dirty = true;

        else {
            String previousLinkCommandPath = plan().previousLinkCommandPath();
            String previousLinkCommand = File::load(previousLinkCommandPath);
            String newLinkCommand = linkJob.command();

            if (newLinkCommand != previousLinkCommand) dirty = true;
            else {
                double productTime = productStatus.lastModified();
                for (const ObjectFile &objectFile: plan().objectFiles()) {
                    FileInfo objectFileStatus = shell().fileStatus(objectFile.objectFilePath());
                    if (objectFileStatus.lastModified() > productTime) {
                        dirty = true;
                        break;
                    }
                }
                if (!dirty) {
                    FileInfo recipeStatus = shell().fileStatus(plan().recipePath());
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
        if (plan().options() & BuildOption::Library) {
            plan().setLibraryLinkJob(linkJob);
        }
        scheduler.schedule(linkJob);
    }
}

void CompileLinkStage::gatherObjects()
{
    const String currentCompileCommand = toolChain().compileCommand(plan(), "%.cc", "%.o");
    String previousCompileCommand;
    try {
        previousCompileCommand = File{plan().previousCompileCommandPath()}.map();
    }
    catch (SystemResourceError &)
    {}

    File{plan().previousCompileCommandPath(), FileOpen::Create|FileOpen::Truncate|FileOpen::WriteOnly}.write(currentCompileCommand);

    const bool compileCommandChanged = currentCompileCommand != previousCompileCommand;

    for (const String &source: plan().sources()) {
        const String objectFilePath = toolChain().objectFilePath(plan(), source);
        const String compileCommand = toolChain().compileCommand(plan(), source, objectFilePath);
        List<String> previousDependencyPaths;
        if (toolChain().readObjectDependencies(objectFilePath, &previousDependencyPaths)) {
            assert(previousDependencyPaths.at(0) == source);
            bool dirty = compileCommandChanged;
            do {
                if (dirty) break;
                FileInfo objectFileInfo = shell().fileStatus(objectFilePath);
                if (!objectFileInfo) { dirty = true; break; }
                for (const String &previousDependencyPath: previousDependencyPaths) {
                    FileInfo previousDependencyInfo = shell().fileStatus(previousDependencyPath);
                    if (!previousDependencyInfo) {
                        dirty = true;
                        break;
                    }
                    if (objectFileInfo.lastModified() < previousDependencyInfo.lastModified()) {
                        dirty = true;
                        break;
                    }
                }
            }
            while (false);
            plan().objectFiles().emplaceBack(compileCommand, objectFilePath, previousDependencyPaths, dirty);
        }
        else {
            plan().objectFiles().emplaceBack(compileCommand, objectFilePath, List<String>{source}, true);
        }
    }
}

} // namespace cc::build
