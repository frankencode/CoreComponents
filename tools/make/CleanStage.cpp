/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "BuildPlan.h"
#include "DependencyCache.h"
#include "CleanStage.h"

namespace fluxmake {

bool CleanStage::run()
{
    if (complete_) return success_;
    complete_ = true;

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

} // namespace fluxmake
