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

class UninstallStage: public BuildStage
{
public:
    UninstallStage(BuildPlan *plan): BuildStage{plan} {}
    bool run();
    bool uninstallTool(Module *module);
    bool uninstallApplicationOrLibrary();
};

} // namespace ccbuild
