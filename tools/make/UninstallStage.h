/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_UNINSTALLSTAGE_H
#define FLUXMAKE_UNINSTALLSTAGE_H

#include "BuildStage.h"

namespace fluxmake {

class UninstallStage: public BuildStage
{
public:
    UninstallStage(BuildPlan *plan): BuildStage(plan) {}
    bool run();
};

} // namespace fluxmake

#endif // FLUXMAKE_UNINSTALLSTAGE_H
