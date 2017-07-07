/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

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

    for (int i = 0; i < plan()->prerequisites()->count(); ++i) {
        if (!plan()->prerequisites()->at(i)->installStage()->run())
            return success_ = false;
    }

    if (plan()->options() & BuildPlan::Package) return success_ = true;

    if (plan()->options() & BuildPlan::Tools) {
        for (int i = 0; i < plan()->modules()->count(); ++i) {
            if (!toolChain()->install(plan(), plan()->modules()->at(i)))
                return success_ = false;
        }
        return success_ = true;
    }

    return success_ = toolChain()->install(plan());
}

} // namespace ccbuild
