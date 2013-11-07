/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXMAKE_BUILDSTAGE_H
#define FLUXMAKE_BUILDSTAGE_H

namespace fmake
{

class BuildPlan;
class BuildShell;
class ToolChain;
class JobScheduler;

class BuildStage
{
public:
	inline bool complete() const { return complete_; }
	inline bool success() const { return success_; }
	inline int status() const { return status_; }

protected:
	BuildStage(BuildPlan *plan);

	inline BuildPlan *plan() const { return plan_; }
	BuildShell *shell() const;
	ToolChain *toolChain() const;
	Ref<JobScheduler> createScheduler() const;

	BuildPlan *plan_;
	bool complete_, success_;
	int status_;
};

} // namespace fmake

#endif // FLUXMAKE_BUILDSTAGE_H
