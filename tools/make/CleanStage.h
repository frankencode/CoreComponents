/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_CLEANSTAGE_H
#define FLUXMAKE_CLEANSTAGE_H

#include "BuildStage.h"

namespace fluxmake {

class CleanStage: public BuildStage
{
public:
    CleanStage(BuildPlan *plan): BuildStage(plan) {}
    bool run();
};

} // namespace fluxmake

#endif // FLUXMAKE_CLEANSTAGE_H
