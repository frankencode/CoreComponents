/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXMAKE_CONFIGURESTAGE_H
#define FLUXMAKE_CONFIGURESTAGE_H

#include "BuildStage.h"

namespace fmake
{

class ConfigureStage: public BuildStage
{
public:
	ConfigureStage(BuildPlan *plan): BuildStage(plan) {}
	bool run();

private:
	bool findIncludePath(SystemPrerequisite *prerequisite, String *includePath);
	bool findLibraryPath(SystemPrerequisite *prerequisite, String *libraryPath);
};

} // namespace fmake

#endif // FLUXMAKE_CONFIGURESTAGE_H
