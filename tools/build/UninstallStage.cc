/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/UninstallStage>
#include <cc/build/BuildStageGuard>
#include <cc/build/CwdGuard>
#include <cc/build/ToolChain>

namespace cc::build {

bool UninstallStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan().goForBuild()) return success_ = true;

    if (plan().options() & BuildOption::Test) return success_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.uninstallStage().run())
            return success_ = false;
    }

    if (plan().options() & BuildOption::Package) return success_ = true;

    if (plan().options() & BuildOption::Tools) {
        for (const ObjectFile &objectFile: plan().objectFiles()) {
            if (!uninstallTool(objectFile))
                return success_ = false;
        }
        return success_ = true;
    }

    return success_ = uninstallApplicationOrLibrary();
}

bool UninstallStage::uninstallTool(const ObjectFile &objectFile)
{
    String product = toolChain().linkName(objectFile);
    return shell().unlink(toolChain().installDirPath(plan()) / product);
}

bool UninstallStage::uninstallApplicationOrLibrary()
{
    BuildOption options = plan().options();
    String product = toolChain().linkName(plan());
    String installDirPath = toolChain().installDirPath(plan());
    String installFilePath = installDirPath / product;

    if (!shell().unlink(installFilePath)) return false;

    if ((options & BuildOption::Library) && !plan().linkStatic()) {
        {
            CwdGuard guard{installDirPath, plan().shell()};
            toolChain().cleanLibrarySymlinks(plan(), product);
            if (plan().options() & BuildOption::Plugin)
                toolChain().cleanPluginSymlinks(plan(), toolChain().targetName(plan().extensionTarget()));
        }
        shell().unlinkAll(
            plan().projectPath() / "include",
            toolChain().includePrefix(plan())
        );
        shell().unlinkAll(
            plan().projectPath() / "libinclude",
            toolChain().libIncludePrefix(plan())
        );

        String pcName = toolChain().pkgConfigName(plan());
        String pcInstallPath = toolChain().pkgConfigInstallDirPath(plan()) / pcName;
        plan().shell().unlink(pcInstallPath);
    }

    if ((options & BuildOption::Application) && plan().alias().count() > 0) {
        CwdGuard guard{installDirPath, plan().shell()};
        toolChain().cleanAliasSymlinks(plan(), product);
    }

    return true;
}

} // namespace cc::build
