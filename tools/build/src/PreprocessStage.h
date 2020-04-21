/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "BuildStage.h"

namespace ccbuild {

class PreprocessStage: public BuildStage
{
public:
    PreprocessStage(BuildPlan *plan): BuildStage{plan} {}
    bool run();

private:
    void scheduleJobs(JobScheduler *scheduler);
};

} // namespace ccbuild
