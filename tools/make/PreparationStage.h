/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_PREPARATIONSTAGE_H
#define FLUXMAKE_PREPARATIONSTAGE_H

#include "BuildStage.h"

namespace fluxmake {

class PreparationStage: public BuildStage
{
public:
    PreparationStage(BuildPlan *plan): BuildStage(plan) {}
    bool run();
};

} // namespace fluxmake

#endif // FLUXMAKE_PREPARATIONSTAGE_H
