/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "BuildStage.h"

namespace ccbuild {

class TestRunStage: public BuildStage
{
public:
    TestRunStage(BuildPlan *plan);

    bool run();

private:
    void scheduleTests(JobScheduler *scheduler);
};

} // namespace ccbuild
