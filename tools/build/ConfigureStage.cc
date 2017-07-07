/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/Dir>
#include <cc/File>
#include <cc/Process>
#include <cc/SubProcess>
#include "BuildPlan.h"
#include "ConfigureCache.h"
#include "ConfigureStage.h"

namespace ccbuild {

bool ConfigureStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    for (int i = 0; i < plan()->prerequisites()->count(); ++i) {
        BuildPlan *other = plan()->prerequisites()->at(i);
        if (!other->configureStage()->run()) {
            success_ = false;
            if (!(plan()->options() & BuildPlan::Package))
                return false;
        }
    }

    if (!plan()->systemPrerequisitesByName()) return success_;

    for (int i = 0; i < plan()->systemPrerequisitesByName()->count(); ++i)
    {
        String name = plan()->systemPrerequisitesByName()->keyAt(i);
        SystemPrerequisiteList *prerequisiteList = plan()->systemPrerequisitesByName()->valueAt(i);

        for (int j = 0; j < prerequisiteList->count(); ++j)
        {
            SystemPrerequisite *prerequisite = prerequisiteList->at(j);

            Ref<StringList> includePaths = configureShell(prerequisite->includePathConfigure())->simplify()->split(' ');
            Ref<StringList> libraryPaths = configureShell(prerequisite->libraryPathConfigure())->simplify()->split(' ');
            Ref<StringList> compileFlags = configureShell(prerequisite->compileFlagsConfigure())->simplify()->split(' ');
            Ref<StringList> linkFlags = configureShell(prerequisite->linkFlagsConfigure())->simplify()->split(' ');
            Version version = configureShell(prerequisite->versionConfigure());

            includePaths->appendList(prerequisite->includePaths());
            libraryPaths->appendList(prerequisite->libraryPaths());
            compileFlags->appendList(prerequisite->customCompileFlags());
            linkFlags->appendList(prerequisite->customLinkFlags());

            try {
                Version versionMin = prerequisite->versionMin();
                Version versionMax = prerequisite->versionMax();
                if (Version::isValid(versionMin)) {
                    if (version < versionMin)
                        throw(String(Format() << "at least version " << versionMin << " is required (version " << version << " detected)"));
                }
                if (Version::isValid(versionMax)) {
                    if (versionMax < version)
                        throw(String(Format() << "at most version " << versionMax << " is supported (version " << version << " detected)"));
                }

                if (prerequisite->includeTest()->count() > 0) {
                    if (!toolChain()->testInclude(plan(), prerequisite->includeTest()))
                        throw(String("include test failed"));
                }
            }
            catch (String &error) {
                if (prerequisite->optional()) {
                    if (plan()->options() & (BuildPlan::Configure|BuildPlan::Verbose)) {
                        ferr() << name << ": " << error << nl;
                        ferr() << name << ": disabled" << nl;
                    }
                    continue;
                }
                else {
                    if (plan()->options() & (BuildPlan::Configure|BuildPlan::Verbose)) {
                        ferr() << name << ": " << error << nl;
                    }
                    return success_ = false;
                }
            }

            if (plan()->options() & (BuildPlan::Configure|BuildPlan::Verbose)) {
                if (includePaths->count() > 0)
                    ferr() << name << ".include-paths = " << includePaths->join(" ") << nl;
                if (libraryPaths->count() > 0)
                    ferr() << name << ".library-paths = " << libraryPaths->join(" ") << nl;
                if (compileFlags->count() > 0)
                    ferr() << name << ".compile-flags = " << compileFlags->join(" ") << nl;
                if (linkFlags->count() > 0)
                    ferr() << name << ".link-flags = " << linkFlags->join(" ") << nl;
                if (Version::isValid(version))
                    ferr() << name << ".version = " << version << nl;
            }

            plan()->includePaths()->appendList(includePaths);
            plan()->libraryPaths()->appendList(libraryPaths);
            plan()->customCompileFlags()->appendList(compileFlags);
            plan()->customLinkFlags()->appendList(linkFlags);
            plan()->libraries()->appendList(prerequisite->libraries());
        }
    }

    return success_;
}

String ConfigureStage::configureShell(String shellCommand) const
{
    if (shellCommand == "") return "";

    String text;
    if (configureCache()->lookup(shellCommand, &text))
        return text;

    Ref<SubProcess> sub = SubProcess::open(
        SubProcess::params()->setArgs(
            StringList::create()
                << Process::env("SHELL")
                << "-c"
                << shellCommand
        )
    );

    text = sub->readAll()->trim();
    int status = sub->wait();
    if (status != 0) {
        if (plan()->options() & BuildPlan::Verbose) {
            ferr() << "Configure command failed with status = " << status << " (\"" << shellCommand << "\")" << nl;
        }
        return "";
    }

    configureCache()->insert(shellCommand, text);

    return text;
}

} // namespace ccbuild
