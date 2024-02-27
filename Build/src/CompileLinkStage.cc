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
#include <cc/build/CodyServer>
#include <cc/build/ImportManager>
#include <cc/File>
#include <cc/stdio>

namespace cc::build {

bool CompileLinkStage::run()
{
    BuildStageGuard guard{this};

    JobScheduler scheduler = createScheduler();

    CodyServer codyServer;

    if (!(plan().options() & (BuildOption::Simulate | BuildOption::Blindfold | BuildOption::Bootstrap))) {
        codyServer = CodyServer{plan(), scheduler};
        plan().mutableToolChain().setCxxModuleMapper(codyServer.connectionInfo());
        codyServer.start();
    }

    if (!scheduleJobs(scheduler)) return success_ = false;

    for (Job job; scheduler.collect(&job);) {
        fout() << shell().beautify(job.command()) << nl;
        ferr() << job.outputText();
        if (job.status() != 0) {
            status_ = job.status();
            return success_ = false;
        }
    }

    if (codyServer) {
        CodyError error;
        codyServer.shutdown(&error);
        if (error) ferr() << error << nl;
        return success_ = !error;
    }

    return success_ = true;
}

bool CompileLinkStage::scheduleJobs(JobScheduler &scheduler)
{
    if (complete_) return true;
    complete_ = true;

    if (!plan().goForBuild()) return true;
    if (plan().isSystemSource()) return true;

    if (plan().options() & BuildOption::Test) {
        if (!(plan().options() & BuildOption::BuildTests))
            return true;
    }

    if (outOfScope()) return true;

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        prerequisite.compileLinkStage().scheduleJobs(scheduler);
    }

    if (plan().options() & BuildOption::Package) return true;

    shell().mkdir(plan().objectsPath());

    if (!gatherUnits()) return false;

    Job linkJob;
    if (!(plan().options() & BuildOption::Tools)) {
        linkJob = toolChain().createLinkJob(plan());
    }

    InsightDatabase insightDatabase;
    if (plan().options() & BuildOption::Insight) {
        insightDatabase = InsightDatabase{plan().projectPath()};
    }

    bool dirty = false;

    for (Unit &unit: plan().units())
    {
        bool unitDirty = unit.dirty();

        if (plan().options() & BuildOption::Tools) {
            unitDirty = unitDirty || !shell().fileStatus(toolChain().linkName(unit));
        }

        if (unitDirty)
        {
            dirty = true;

            Job job;
            if (plan().options() & BuildOption::Tools) {
                job = toolChain().createCompileLinkJob(plan(), unit);
                if (!(plan().options() & BuildOption::Simulate)) {
                    plan().registerLinkDerivative(job);
                }
            }
            else {
                job = toolChain().createCompileJob(plan(), unit);
                job.registerDerivative(linkJob);
            }

            if (plan().options() & BuildOption::Simulate) {
                fout() << plan().shell().beautify(job.command()) << ((plan().concurrency() == 1) ? "\n" : " &\n");
            }
            else {
                if (unit.module()) unit.module()->job_ = job;
                scheduler.schedule(job);
            }

            if (insightDatabase) {
                insightDatabase.insert(unit.source(), job.command(), unit.target());
            }
        }
        else if (insightDatabase) {
            insightDatabase.touch(unit.target());
        }
    }

    if (insightDatabase) {
        insightDatabase.sync();

        if (plan().options() & BuildOption::Verbose) {
            ferr() << "Updated insight database " << insightDatabase.path() << nl;
        }
    }

    if (
        plan().units().count() > 0 &&
        (plan().options() & BuildOption::Simulate) &&
        plan().concurrency() != 1
    ) {
        fout() << "wait" << nl;
    }

    if (plan().options() & BuildOption::Tools) return true;

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
                for (const Unit &unit: plan().units()) {
                    FileInfo objectFileStatus = shell().fileStatus(unit.target());
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

    if (!dirty) return true;

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

    return true;
}

bool CompileLinkStage::gatherUnits()
{
    const String currentCompileCommand = toolChain().compileCommand(plan(), "%.cc", "%.o");
    String previousCompileCommand;
    try {
        previousCompileCommand = File{plan().previousCompileCommandPath()}.map();
    }
    catch (SystemResourceError &)
    {}

    if (!(plan().options() & BuildOption::Simulate)) {
        File{plan().previousCompileCommandPath(), FileOpen::Create|FileOpen::Truncate|FileOpen::WriteOnly}.write(currentCompileCommand);
    }

    const bool compileCommandChanged = currentCompileCommand != previousCompileCommand;

    ImportManager importManager;

    for (const String &source: plan().sources()) {
        const String objectFilePath = toolChain().objectFilePath(plan(), source);
        const String compileCommand = toolChain().compileCommand(plan(), source, objectFilePath);
        List<String> previousDependencyPaths;
        bool dirty = compileCommandChanged;
        Unit unit;
        Module module;
        if (source.startsWith(plan().importPath())) {
            const String name = ImportManager::moduleName(plan().importPath(), source);
            if (!importManager.registerModule(name, source, &module)) {
                ferr() <<
                    "Error, ambiguous definition for module \"" << name << "\":\n"
                    "  " << source << "\n"
                    "  " << module.source() << "\n";

                return false;
            }
        }
        if (toolChain().readObjectDependencies(plan(), objectFilePath, &previousDependencyPaths)) {
            assert(previousDependencyPaths(0) == source);
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

            unit = Unit{compileCommand, objectFilePath, previousDependencyPaths(0), dirty};
        }
        else {
            unit = Unit{compileCommand, objectFilePath, List<String>{source}, true};
        }

        plan().units().append(unit);
        // if (module) module->unit_ = unit; // TODO...
    }

    return true;
}

} // namespace cc::build
