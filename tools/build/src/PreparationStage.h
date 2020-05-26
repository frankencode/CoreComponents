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

class PreparationStage: public BuildStage
{
public:
    PreparationStage(BuildPlan *plan): BuildStage{plan} {}
    bool run();

private:
    static string expand(const string &command, const string &sourcePath, const string &targetPath);
};

} // namespace ccbuild
