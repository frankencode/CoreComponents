/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/InstallStage>
#include <cc/build/BuildStageGuard>
#include <cc/build/CwdGuard>
#include <cc/build/ToolChain>
#include <cc/File>

namespace cc::build {

bool InstallStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan().goForBuild()) return success_ = true;
    if (plan().isSystemSource()) return success_ = true;

    if (outOfScope()) return success_ = true;
    if (plan().options() & BuildOption::Test) return success_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.projectPath().startsWith(plan().projectPath())) continue;
        if (!prerequisite.installStage().run()) return success_ = false;
        linkerCacheDirty_ = linkerCacheDirty_ || prerequisite.installStage().linkerCacheDirty_;
    }

    if (plan().options() & BuildOption::Package) return success_ = true;

    if (plan().options() & BuildOption::Tools) {
        for (const Module &module: plan().modules()) {
            if (!installTool(module))
                return success_ = false;
        }
        return success_ = true;
    }

    return success_ = installApplicationOrLibrary();
}

bool InstallStage::installTool(const Module &module)
{
    String product = toolChain().linkName(module);
    return shell().install(product, toolChain().installDirPath(plan()) / product);
}

bool InstallStage::installApplicationOrLibrary()
{
    BuildOption options = plan().options();
    String product = toolChain().linkName(plan());
    String installDirPath = toolChain().installDirPath(plan());
    String installFilePath = installDirPath / product;

    if (!shell().install(product, installFilePath)) return false;

    if (options & BuildOption::Application) {
        if (plan().alias().count() > 0) {
            CwdGuard guard{installDirPath, shell()};
            toolChain().createAliasSymlinks(plan(), product);
        }
    }
    else if (options & BuildOption::Library)
    {
        if (!plan().linkStatic()) {
            linkerCacheDirty_ = true;
            CwdGuard guard{installDirPath, shell()};
            toolChain().createLibrarySymlinks(plan(), product);
            if (options & BuildOption::Plugin)
                toolChain().createPluginSymlinks(plan(), toolChain().linkName(plan().extensionTarget()).baseName(), product);
        }

        if (!
            shell().installAll(
                plan().projectPath() / "include",
                toolChain().includePrefix(plan())
            )
        )
            return false;

        if (!
            shell().installAll(
                plan().projectPath() / "libinclude",
                toolChain().libIncludePrefix(plan())
            )
        )
            return false;

        String pcName = toolChain().pkgConfigName(plan());
        String pcInstallPath = toolChain().pkgConfigInstallDirPath(plan()) / pcName;
        if (File::exists(plan().userPkgConfigPath())) {
            if (!shell().install(plan().userPkgConfigPath(), pcInstallPath))
                return false;
        }
        else {
            if (!File::exists(pcName)) toolChain().generatePkgConfig(plan());
            if (!shell().install(pcName, pcInstallPath))
                return false;
        }
    }

    for (const String &bundlePath: plan().bundle()) {
        String relativePath = bundlePath.copy(plan().projectPath().count() + 1, bundlePath.count());
        shell().install(
            bundlePath,
            plan().installRoot() / toolChain().bundlePrefix(plan()) / relativePath
        );
    }

    return true;
}

} // namespace cc::build
