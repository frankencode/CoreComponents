/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FMAKE_ANALYSESTAGE_H
#define FMAKE_ANALYSESTAGE_H

#include <fkit/String.h>
#include "BuildStage.h"

namespace fmake
{

class AnalyseStage: public BuildStage
{
public:
	bool run();
	inline StringList *previousSources() const { return previousSources_; }

protected:
	AnalyseStage(BuildPlan *plan): BuildStage(plan) {}

private:
	Ref<StringList> previousSources_;
};

} // namespace fmake

#endif // FMAKE_ANALYSESTAGE_H
