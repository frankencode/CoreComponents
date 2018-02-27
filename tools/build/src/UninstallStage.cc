/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/exceptions>
#include "CwdGuard.h"
#include "BuildPlan.h"
#include "UninstallStage.h"

namespace ccbuild {

bool UninstallStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan()->goForBuild()) return success_ = true;

    if (plan()->options() & BuildPlan::Test) return success_ = true;

    BuildStageGuard guard(this);

    for (BuildPlan *prerequisite: plan()->prerequisites()) {
        if (!prerequisite->uninstallStage()->run())
            return success_ = false;
    }

    if (plan()->options() & BuildPlan::Package) return success_ = true;

    if (plan()->options() & BuildPlan::Tools) {
        for (Module *module: plan()->modules()) {
            if (!uninstallTool(module))
                return success_ = false;
        }
        return success_ = true;
    }

    return success_ = uninstallApplicationOrLibrary();
}

bool UninstallStage::uninstallTool(Module *module)
{
    String product = module->toolName();
    return shell()->unlink(toolChain()->installDirPath(plan())->extendPath(product));
}

bool UninstallStage::uninstallApplicationOrLibrary()
{
    int options = plan()->options();
    String product = toolChain()->linkName(plan());
    String installDirPath = toolChain()->installDirPath(plan());
    String installFilePath = installDirPath->extendPath(product);

    if (!shell()->unlink(installFilePath)) return false;

    if ((options & BuildPlan::Library) && !plan()->linkStatic()) {
        {
            CwdGuard guard(installDirPath, plan()->shell());
            toolChain()->cleanLibrarySymlinks(plan(), product);
            if (plan()->options() & BuildPlan::Plugin)
                toolChain()->cleanPluginSymlinks(plan(), toolChain()->targetName(plan()->extensionTarget()));
        }
        shell()->unlinkAll(
            plan()->projectPath()->extendPath("include"),
            toolChain()->includePrefix(plan())
        );
        shell()->unlinkAll(
            plan()->projectPath()->extendPath("libinclude"),
            toolChain()->libIncludePrefix(plan())
        );

        String pcName = toolChain()->pkgConfigName(plan());
        String pcInstallPath = toolChain()->pkgConfigInstallDirPath(plan())->extendPath(pcName);
        plan()->shell()->unlink(pcInstallPath);
    }

    if ((options & BuildPlan::Application) && plan()->alias()->count() > 0) {
        CwdGuard guard(installDirPath, plan()->shell());
        toolChain()->cleanAliasSymlinks(plan(), product);
    }

    return true;
}

} // namespace ccbuild
