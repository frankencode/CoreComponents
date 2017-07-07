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

    for (int i = 0; i < plan()->prerequisites()->count(); ++i) {
        if (!plan()->prerequisites()->at(i)->cleanStage()->run())
            return success_ = false;
    }

    if (plan()->options() & BuildPlan::Package) return success_ = true;

    toolChain()->clean(plan());
    shell()->unlink(DependencyCache::cachePath(plan()));
    shell()->rmdir(plan()->modulePath());

    return success_ = true;
}

} // namespace ccbuild
