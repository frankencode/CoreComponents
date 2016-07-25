/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "BuildPlan.h"
#include "UninstallStage.h"

namespace ccbuild {

bool UninstallStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan()->goForBuild()) return success_ = true;

    if (plan()->options() & BuildPlan::Test) return success_ = true;

    for (int i = 0; i < plan()->prerequisites()->count(); ++i) {
        if (!plan()->prerequisites()->at(i)->uninstallStage()->run())
            return success_ = false;
    }

    if (plan()->options() & BuildPlan::Package) return success_ = true;

    if (plan()->options() & BuildPlan::Tools) {
        for (int i = 0; i < plan()->modules()->count(); ++i) {
            if (!toolChain()->uninstall(plan(), plan()->modules()->at(i)))
                return success_ = false;
        }
        return success_ = true;
    }

    return success_ = toolChain()->uninstall(plan());
}

} // namespace ccbuild
