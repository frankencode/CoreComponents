/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "BuildStage.h"

namespace ccbuild {

class CompileLinkStage: public BuildStage
{
public:
    CompileLinkStage(BuildPlan *plan): BuildStage(plan) {}
    bool run();
};

} // namespace ccbuild

