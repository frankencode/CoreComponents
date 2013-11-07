/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXMAKE_ANALYSESTAGE_H
#define FLUXMAKE_ANALYSESTAGE_H

#include <flux/String.h>
#include "BuildStage.h"

namespace fluxmake
{

class AnalyseStage: public BuildStage
{
public:
	AnalyseStage(BuildPlan *plan): BuildStage(plan) {}
	bool run();
	inline StringList *previousSources() const { return previousSources_; }

private:
	Ref<StringList> previousSources_;
};

} // namespace fluxmake

#endif // FLUXMAKE_ANALYSESTAGE_H
