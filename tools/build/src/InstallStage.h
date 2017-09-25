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

class InstallStage: public BuildStage
{
public:
    InstallStage(BuildPlan *plan): BuildStage(plan), linkerCacheDirty_(false) {}

    bool run();
    bool installTool(Module *module);
    bool installApplicationOrLibrary();
    bool installPkgConfig();

    bool linkerCacheDirty() const { return linkerCacheDirty_; }

private:
    bool linkerCacheDirty_;
};

} // namespace ccbuild
