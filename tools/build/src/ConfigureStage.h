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

class ConfigureStage: public BuildStage
{
public:
    ConfigureStage(BuildPlan *plan): BuildStage(plan) {}
    bool run();

    String configureShell(String shellCommand) const;
};

} // namespace ccbuild

