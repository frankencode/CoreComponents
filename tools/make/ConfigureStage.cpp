/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "BuildPlan.h"
#include "ConfigureStage.h"

namespace fmake
{

bool ConfigureStage::run()
{
	if (complete_) return success_;
	complete_ = true;

	// bool forceReconfiguration = plan()->recipe()->value("configure");

	return success_ = true;
}

} // namespace fmake
