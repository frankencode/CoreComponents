/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FMAKE_INSTALLSTAGE_H
#define FMAKE_INSTALLSTAGE_H

#include "BuildStage.h"

namespace fmake
{

class InstallStage: public BuildStage
{
public:
	InstallStage(BuildPlan *plan): BuildStage(plan) {}
	bool run();
};

} // namespace fmake

#endif // FMAKE_INSTALLSTAGE_H
