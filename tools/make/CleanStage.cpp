/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "BuildPlan.h"
#include "DependencyCache.h"
#include "CleanStage.h"

namespace fmake
{

bool CleanStage::run()
{
	if (complete_) return success_;
	complete_ = true;

	for (int i = 0; i < plan()->prerequisites()->size(); ++i) {
		if (!plan()->prerequisites()->at(i)->cleanStage()->run())
			return success_ = false;
	}

	if (plan()->options() & BuildPlan::Package) return success_ = true;

	toolChain()->clean(plan());
	shell()->unlink(DependencyCache::cachePath(plan()));
	shell()->rmdir(plan()->modulePath());

	return success_ = true;
}

} // namespace fmake
