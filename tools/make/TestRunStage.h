/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_TESTRUNSTAGE_H
#define FLUXMAKE_TESTRUNSTAGE_H

#include "BuildStage.h"

namespace fluxmake {

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

} // namespace fluxmake

#endif // FLUXMAKE_TESTRUNSTAGE_H
