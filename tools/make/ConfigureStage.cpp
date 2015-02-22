/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/Dir>
#include <flux/File>
#include <flux/Process>
#include <flux/ProcessFactory>
#include "BuildPlan.h"
#include "ConfigureCache.h"
#include "ConfigureStage.h"

namespace fluxmake {

bool ConfigureStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    for (int i = 0; i < plan()->prerequisites()->count(); ++i) {
        if (!plan()->prerequisites()->at(i)->configureStage()->run())
            return success_ = false;
    }

    if (!plan()->systemPrerequisitesByName()) return success_ = true;

    for (int i = 0; i < plan()->systemPrerequisitesByName()->count(); ++i)
    {
        String name = plan()->systemPrerequisitesByName()->keyAt(i);
        SystemPrerequisiteList *prerequisiteList = plan()->systemPrerequisitesByName()->valueAt(i);

        for (int j = 0; j < prerequisiteList->count(); ++j)
        {
            SystemPrerequisite *prerequisite = prerequisiteList->at(j);
            String includePath;
            if (prerequisite->includePathConfigure() != "") {
                includePath = configureShell(prerequisite->includePathConfigure());
            }
            else if (!findIncludePath(prerequisite, &includePath)) {
                if (!prerequisite->optional()) {
                    ferr() << "Missing system prerequisite \"" << prerequisite->name() << "\": Include path not found" << nl;
                    return success_ = false;
                }
            }
            String libraryPath;
            if (prerequisite->libraryPathConfigure() != "") {
                libraryPath = configureShell(prerequisite->libraryPathConfigure());
            }
            else if (!findLibraryPath(prerequisite, &libraryPath)) {
                if (!prerequisite->optional()) {
                    ferr() << "Missing system prerequisite \"" << prerequisite->name() << "\": Library path not found" << nl;
                    return success_ = false;
                }
            }
            if ((plan()->options() & BuildPlan::Configure) && includePath != "")
                ferr() << "Include path for " << name << ": " << includePath << nl;
            if ((plan()->options() & BuildPlan::Configure) && libraryPath != "")
                ferr() << "Library path for " << name << ": " << libraryPath << nl;

            if (includePath != "") plan()->includePaths()->append(includePath);
            if (libraryPath != "") plan()->libraryPaths()->append(libraryPath);

            plan()->customCompileFlags()->appendList(prerequisite->customCompileFlags());
            plan()->customLinkFlags()->appendList(prerequisite->customLinkFlags());
            plan()->libraries()->appendList(prerequisite->libraries());
        }
    }

    return success_ = true;
}

String ConfigureStage::configureShell(String shellCommand) const
{
    Ref<ProcessFactory> factory = ProcessFactory::create(Process::GroupLeader);
    String shell = Process::env("SHELL");
    factory->setExecPath(shell);
    factory->setArguments(
        StringList::create()
            << shell
            << "-c"
            << shellCommand
    );
    factory->setIoPolicy(Process::ForwardOutput);

    String text;
    if (configureCache()->lookup(shellCommand, &text))
        return text;

    Ref<Process> process = factory->produce();
    text = process->readAll()->trim();
    int status = process->wait();
    if (status != 0) {
        ferr() << "Configure command failed with status = " << status << " (\"" << shellCommand << "\")" << nl;
        return "";
    }

    configureCache()->insert(shellCommand, text);

    return text;
}

bool ConfigureStage::findIncludePath(SystemPrerequisite *prerequisite, String *includePath)
{
    if (prerequisite->includePaths()->count() == 1) {
        *includePath = prerequisite->includePaths()->at(0);
        return true;
    }
    for (int i = 0; i < prerequisite->includePaths()->count(); ++i) {
        String path = prerequisite->includePaths()->at(i);
        if (plan()->toolChain()->includeTest(plan(), path, prerequisite->testIncludes())) {
            *includePath = path;
            return true;
        }
    }
    *includePath = "";
    return prerequisite->includePaths()->count() == 0;
}

bool ConfigureStage::findLibraryPath(SystemPrerequisite *prerequisite, String *libraryPath)
{
    for (int i = 0; i < prerequisite->libraryPaths()->count(); ++i) {
        String path = prerequisite->libraryPaths()->at(i);
        if (plan()->toolChain()->linkTest(plan(), path, prerequisite->testLibraries())) {
            *libraryPath = path;
            return true;
        }
    }
    *libraryPath = "";
    return prerequisite->libraryPaths()->count() == 0;
}

} // namespace fluxmake
