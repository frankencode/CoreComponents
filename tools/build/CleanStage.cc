/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/CleanStage>
#include <cc/build/BuildStageGuard>
#include <cc/build/BuildShell>
#include <cc/build/ToolChain>

namespace cc::build {

bool CleanStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan().goForBuild()) return success_ = true;
    if (plan().isSystemSource()) return success_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.cleanStage().run())
            return success_ = false;
    }

    if (plan().options() & BuildOption::Package) return success_ = true;

    if (!shell().clean(plan().modulePath())) return success_ = false;

    String product = toolChain().linkName(plan());

    if ((plan().options() & BuildOption::Application) || (plan().options() & BuildOption::Library))
        shell().unlink(product);

    if (plan().options() & BuildOption::Application)
        toolChain().cleanAliasSymlinks(plan(), product);

    if (plan().options() & BuildOption::Library) {
        if (!plan().linkStatic()) {
            toolChain().cleanLibrarySymlinks(plan(), product);
            if (plan().options() & BuildOption::Plugin)
                toolChain().cleanPluginSymlinks(plan(), toolChain().targetName(plan().extensionTarget()));
        }
    }

    if (plan().options() & BuildOption::Tools) {
        for (const Module &module: plan().modules())
            shell().unlink(toolChain().linkName(module));
    }

    return success_ = true;
}

} // namespace cc::build
