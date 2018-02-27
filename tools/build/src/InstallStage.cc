/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Dir>
#include <cc/File>
#include "CwdGuard.h"
#include "BuildPlan.h"
#include "InstallStage.h"

namespace ccbuild {

bool InstallStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan()->goForBuild()) return success_ = true;
    if (plan()->isSystemSource()) return success_ = true;

    if (outOfScope()) return success_ = true;
    if (plan()->options() & BuildPlan::Test) return success_ = true;

    BuildStageGuard guard(this);

    for (BuildPlan *prerequisite: plan()->prerequisites()) {
        if (!prerequisite->installStage()->run())
            return success_ = false;
        linkerCacheDirty_ = linkerCacheDirty_ || prerequisite->installStage()->linkerCacheDirty_;
    }

    if (plan()->options() & BuildPlan::Package) return success_ = true;

    if (plan()->options() & BuildPlan::Tools) {
        for (Module *module: plan()->modules()) {
            if (!installTool(module))
                return success_ = false;
        }
        return success_ = true;
    }

    return success_ = installApplicationOrLibrary();
}

bool InstallStage::installTool(Module *module)
{
    String product = module->toolName();
    return shell()->install(product, toolChain()->installDirPath(plan())->extendPath(product));
}

bool InstallStage::installApplicationOrLibrary()
{
    int options = plan()->options();
    String product = toolChain()->linkName(plan());
    String installDirPath = toolChain()->installDirPath(plan());
    String installFilePath = installDirPath->extendPath(product);

    if (!shell()->install(product, installFilePath)) return false;

    if (options & BuildPlan::Application) {
        if (plan()->alias()->count() > 0) {
            CwdGuard guard(installDirPath, shell());
            toolChain()->createAliasSymlinks(plan(), product);
        }
    }
    else if (options & BuildPlan::Library)
    {
        if (!plan()->linkStatic()) {
            linkerCacheDirty_ = true;
            CwdGuard guard(installDirPath, shell());
            toolChain()->createLibrarySymlinks(plan(), product);
            if (options & BuildPlan::Plugin)
                toolChain()->createPluginSymlinks(plan(), toolChain()->targetName(plan()->extensionTarget()), product);
        }

        if (!
            shell()->installAll(
                plan()->projectPath()->extendPath("include"),
                toolChain()->includePrefix(plan())
            )
        )
            return false;

        if (!
            shell()->installAll(
                plan()->projectPath()->extendPath("libinclude"),
                toolChain()->libIncludePrefix(plan())
            )
        )
            return false;

        String pcName = toolChain()->pkgConfigName(plan());
        String pcInstallPath = toolChain()->pkgConfigInstallDirPath(plan())->extendPath(pcName);
        if (File::exists(plan()->userPkgConfigPath())) {
            if (!shell()->install(plan()->userPkgConfigPath(), pcInstallPath))
                return false;
        }
        else {
            if (!File::exists(pcName)) toolChain()->generatePkgConfig(plan());
            if (!shell()->install(pcName, pcInstallPath))
                return false;
        }
    }

    for (String bundlePath: plan()->bundle()) {
        String relativePath = bundlePath->copy(plan()->projectPath()->count() + 1, bundlePath->count());
        shell()->install(
            bundlePath,
            plan()->installRoot()->extendPath(toolChain()->bundlePrefix(plan())->extendPath(relativePath))
        );
    }

    return true;
}

} // namespace ccbuild
