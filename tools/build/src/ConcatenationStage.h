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

class ConcatenationStage: public BuildStage
{
public:
    ConcatenationStage(BuildPlan *plan): BuildStage{plan} {}
    bool run();
};

} // namespace ccbuild
