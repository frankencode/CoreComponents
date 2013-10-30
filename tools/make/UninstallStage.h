/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FMAKE_UNINSTALLSTAGE_H
#define FMAKE_UNINSTALLSTAGE_H

#include "BuildStage.h"

namespace fmake
{

class UninstallStage: public BuildStage
{
public:
	UninstallStage(BuildPlan *plan): BuildStage(plan) {}
	bool run();
};

} // namespace fmake

#endif // FMAKE_UNINSTALLSTAGE_H
