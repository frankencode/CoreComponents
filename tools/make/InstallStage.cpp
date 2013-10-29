/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "BuildPlan.h"
#include "InstallStage.h"

namespace fmake
{

bool InstallStage::run()
{
	if (complete_) return success_;
	complete_ = true;

	if (plan()->options() & BuildPlan::Tests) return success_ = true;

	for (int i = 0; i < plan()->prerequisites()->size(); ++i)
		if (!plan()->prerequisites()->at(i)->installStage()->run()) return success_ = false;

	if (plan()->options() & BuildPlan::Package) return success_ = true;

	if (plan()->options() & BuildPlan::Tools) {
		for (int i = 0; i < plan()->modules()->size(); ++i) {
			if (!toolChain()->install(plan(), plan()->modules()->at(i)))
				return success_ = false;
		}
		return success_ = true;
	}

	return success_ = toolChain()->install(plan());
}

} // namespace fmake
