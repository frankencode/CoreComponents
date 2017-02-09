/*
 * Copyright (C) 2007-2016 Frank Mertens.
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
    AnalyseStage(BuildPlan *plan): BuildStage(plan), cacheTime_(0) {}
    bool run();
    inline StringList *previousSources() const { return previousSources_; }
    inline double cacheTime() const { return cacheTime_; }

private:
    Ref<StringList> previousSources_;
    double cacheTime_;
};

} // namespace ccbuild

