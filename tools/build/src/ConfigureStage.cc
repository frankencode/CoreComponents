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
#include "BuildPlan.h"
#include "ConfigureShell.h"
#include "ConfigureStage.h"

namespace ccbuild {

bool ConfigureStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    BuildStageGuard guard(this);

    for (BuildPlan *prerequisite: plan()->prerequisites()) {
        if (!prerequisite->configureStage()->run()) {
            success_ = false;
            if (!(plan()->options() & BuildPlan::Package))
                return false;
        }
        makeUseOf(prerequisite);
    }

    if (plan()->usage())
        plan()->BuildParameters::readSpecific(plan()->usage()); // FIXME: obsolete starting from v0.14.0

    if (!plan()->systemPrerequisitesByName()) return success_;

    bool firstLine = true;

    for (int i = 0; i < plan()->systemPrerequisitesByName()->count(); ++i)
    {
        String name = plan()->systemPrerequisitesByName()->keyAt(i);
        SystemPrerequisiteList *prerequisiteList = plan()->systemPrerequisitesByName()->valueAt(i);

        for (SystemPrerequisite *prerequisite: prerequisiteList)
        {
            Version version;

            if (prerequisite->autoConfigure()) {
                prerequisite->customCompileFlags()->appendList(
                    configureShell(String("pkg-config --cflags ") + prerequisite->name())->simplify()->split(' ')
                );
                prerequisite->customLinkFlags()->appendList(
                    configureShell(String("pkg-config --libs ") + prerequisite->name())->simplify()->split(' ')
                );
                version = configureShell(String("pkg-config --modversion ") + prerequisite->name());
            }
            else {
                prerequisite->customCompileFlags()->appendList(
                    configureShell(prerequisite->compileFlagsConfigure())->simplify()->split(' ')
                );
                prerequisite->customLinkFlags()->appendList(
                    configureShell(prerequisite->linkFlagsConfigure())->simplify()->split(' ')
                );
                version = configureShell(prerequisite->versionConfigure());
            }

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

                if (prerequisite->includeTest()->count() > 0) { // FIXME: obsolete starting from v0.14.0
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
                if (firstLine) {
                    firstLine = false;
                    ferr() << plan()->projectPath() << ":" << nl;
                }
                if (prerequisite->customLinkFlags()->count() > 0)
                    ferr() << "  " << name << ".compile-flags = " << prerequisite->customLinkFlags()->join(" ") << nl;
                if (prerequisite->customLinkFlags()->count() > 0)
                    ferr() << "  " << name << ".link-flags = " << prerequisite->customLinkFlags()->join(" ") << nl;
                if (Version::isValid(version))
                    ferr() << "  " << name << ".version = " << version << nl;
            }

            if (prerequisite->libraries()) plan()->libraries()->appendList(prerequisite->libraries());
            plan()->customCompileFlags()->appendList(prerequisite->customCompileFlags());
            plan()->customLinkFlags()->appendList(prerequisite->customLinkFlags());
        }
    }

    StringList::makeUnique(plan()->customCompileFlags());
    StringList::makeUnique(plan()->customLinkFlags());

    return success_;
}

String ConfigureStage::configureShell(String shellCommand)
{
    return ConfigureShell::instance()->run(shellCommand);
}

void ConfigureStage::makeUseOf(BuildPlan *other)
{
    if (other->options() & BuildPlan::Library) {
        String path = other->projectPath();
        String defaultIncludePath = path->expandPath("include");
        if (Dir::exists(defaultIncludePath)) {
            if (!plan()->includePaths()->contains(defaultIncludePath))
                plan()->includePaths()->append(defaultIncludePath);
        }
        else if (!plan()->includePaths()->contains(path)) {
            plan()->includePaths()->append(path);
        }
        plan()->libraries()->append(other->name());

        if (other->usage()) plan()->BuildParameters::readSpecific(other->usage()); // FIXME: obsolete starting from v0.14.0

        if (other->systemPrerequisitesByName()) {
            for (int i = 0; i < other->systemPrerequisitesByName()->count(); ++i)
            {
                SystemPrerequisiteList *prerequisiteList = other->systemPrerequisitesByName()->valueAt(i);

                for (SystemPrerequisite *prerequisite: prerequisiteList)
                {
                    if (prerequisite->cascade()) {
                        plan()->customCompileFlags()->appendList(prerequisite->customCompileFlags());
                        plan()->customLinkFlags()->appendList(prerequisite->customLinkFlags());
                    }
                }
            }
        }
    }
    else if (other->options() & BuildPlan::Package) {
        for (BuildPlan *prerequisite: other->prerequisites())
            makeUseOf(prerequisite);
    }
}

} // namespace ccbuild
