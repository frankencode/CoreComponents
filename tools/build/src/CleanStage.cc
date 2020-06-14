/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "BuildPlan.h"
#include "DependencyCache.h"
#include "CleanStage.h"

namespace ccbuild {

bool CleanStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan()->goForBuild()) return success_ = true;
    if (plan()->isSystemSource()) return success_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan *prerequisite: plan()->prerequisites()) {
        if (!prerequisite->cleanStage()->run())
            return success_ = false;
    }

    if (plan()->options() & BuildPlan::Package) return success_ = true;

    if (!shell()->clean(plan()->modulePath())) return success_ = false;

    String product = toolChain()->linkName(plan());

    if ((plan()->options() & BuildPlan::Application) || (plan()->options() & BuildPlan::Library))
        shell()->unlink(product);

    if (plan()->options() & BuildPlan::Application)
        toolChain()->cleanAliasSymlinks(plan(), product);

    if (plan()->options() & BuildPlan::Library) {
        if (!plan()->linkStatic()) {
            toolChain()->cleanLibrarySymlinks(plan(), product);
            if (plan()->options() & BuildPlan::Plugin)
                toolChain()->cleanPluginSymlinks(plan(), toolChain()->targetName(plan()->extensionTarget()));
        }
    }

    if (plan()->options() & BuildPlan::Tools) {
        for (const Module *module: plan()->modules())
            shell()->unlink(module->toolName());
    }

    return success_ = true;
}

} // namespace ccbuild
