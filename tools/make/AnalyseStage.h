/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
