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
    AnalyseStage(BuildPlan *plan): BuildStage(plan) {}
    bool run();
    inline StringList *previousSources() const { return previousSources_; }

private:
    Ref<StringList> previousSources_;
};

} // namespace ccbuild

