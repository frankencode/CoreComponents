/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include "BuildStage.h"

namespace ccbuild {

class AnalyseStage: public BuildStage
{
public:
    AnalyseStage(BuildPlan *plan): BuildStage{plan}, cacheTime_{0} {}
    bool run();
    StringList previousSources() const { return previousSources_; }
    double cacheTime() const { return cacheTime_; }

private:
    StringList previousSources_;
    double cacheTime_;
};

} // namespace ccbuild
