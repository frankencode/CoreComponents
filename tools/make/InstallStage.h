/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXMAKE_INSTALLSTAGE_H
#define FLUXMAKE_INSTALLSTAGE_H

#include "BuildStage.h"

namespace fluxmake
{

class InstallStage: public BuildStage
{
public:
	InstallStage(BuildPlan *plan): BuildStage(plan) {}
	bool run();
};

} // namespace fluxmake

#endif // FLUXMAKE_INSTALLSTAGE_H
