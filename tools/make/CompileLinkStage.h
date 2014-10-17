/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_COMPILELINKSTAGE_H
#define FLUXMAKE_COMPILELINKSTAGE_H

#include "BuildStage.h"

namespace fluxmake {

class CompileLinkStage: public BuildStage
{
public:
    CompileLinkStage(BuildPlan *plan): BuildStage(plan) {}
    bool run();
};

} // namespace fluxmake

#endif // FLUXMAKE_COMPILELINKSTAGE_H
