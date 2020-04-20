/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include "BuildPlan.h"
#include "ConcatenationStage.h"

namespace ccbuild {

bool ConcatenationStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan()->goForBuild()) return success_ = true;
    if (plan()->isSystemSource()) return success_ = true;

    if (outOfScope()) return success_ = true;
    if (plan()->options() & BuildPlan::Test) return success_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan *prerequisite: plan()->prerequisites())
        prerequisite->concatenationStage()->run();

    // TODO:
    //   * for each module
    //     * grab source code
    //     * for each of the module's dependency paths try to resolve implementation path (move upwards)
    //       * if found replace all includes by dependency header and append implementation to the module's source
    //   * concatenate all the module's sources
    //   * append all prequisite's concatentations
    //   * output a TargetName.cat (which makes only sense for application or library targets)

    return true;
}

} // namespace ccbuild
