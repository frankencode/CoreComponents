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

class CleanStage: public BuildStage
{
public:
    CleanStage(BuildPlan *plan): BuildStage{plan} {}
    bool run();
};

} // namespace ccbuild
