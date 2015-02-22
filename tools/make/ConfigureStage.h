/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_CONFIGURESTAGE_H
#define FLUXMAKE_CONFIGURESTAGE_H

#include "BuildStage.h"

namespace fluxmake {

class ConfigureStage: public BuildStage
{
public:
    ConfigureStage(BuildPlan *plan): BuildStage(plan) {}
    bool run();

private:
    String configureShell(String shellCommand) const;
    bool findIncludePath(SystemPrerequisite *prerequisite, String *includePath);
    bool findLibraryPath(SystemPrerequisite *prerequisite, String *libraryPath);
};

} // namespace fluxmake

#endif // FLUXMAKE_CONFIGURESTAGE_H
