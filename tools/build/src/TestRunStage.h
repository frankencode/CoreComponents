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

class TestRunStage: public BuildStage
{
public:
    TestRunStage(BuildPlan *plan);

    inline int testTotal() const { return testTotal_; }
    inline int testFailed() const { return testFailed_; }

    bool run();

private:
    bool runTests(bool report);

    int testTotal_, testFailed_;
};

} // namespace ccbuild
