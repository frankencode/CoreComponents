/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FMAKE_TOOLCHAIN_H
#define FMAKE_TOOLCHAIN_H

#include <fkit/Config.h>
#include "Job.h"
#include "Module.h"

namespace fmake
{

using namespace fkit;

FKIT_STD_EXCEPTION(MachException);

class BuildPlan;
class DependencyCache;

class ToolChain: public Object
{
public:
	inline String execPath() const { return execPath_; }
	inline String machine() const { return machine_; }

	virtual String machineCommand() const = 0;
	virtual int defaultSpeedOptimizationLevel() const = 0;
	virtual int defaultSizeOptimizationLevel() const = 0;

	virtual String analyseCommand(BuildPlan *plan, String source) const = 0;
	virtual Ref<Job> createAnalyseJob(BuildPlan *plan, String source) = 0;
	virtual Ref<Module> finishAnalyseJob(BuildPlan *plan, Job *job) = 0;

	virtual Ref<Job> createCompileJob(BuildPlan *plan, Module *module) = 0;
	virtual Ref<Job> createLinkJob(BuildPlan *plan, Module *module) = 0;
	virtual Ref<Job> createTestJob(BuildPlan *plan, Module *module) = 0;

	virtual String linkName(BuildPlan *plan) const = 0;
	virtual bool link(BuildPlan *plan) = 0;

	virtual bool install(BuildPlan *plan) = 0;
	virtual bool install(BuildPlan *plan, Module *module) = 0;
	virtual bool uninstall(BuildPlan *plan) = 0;
	virtual bool uninstall(BuildPlan *plan, Module *module) = 0;

	virtual void clean(BuildPlan *plan) = 0;

protected:
	ToolChain(String execPath, String machine)
		: execPath_(execPath),
		  machine_(machine)
	{}

private:
	String execPath_;
	String machine_;
};

} // namespace fmake

#endif // FMAKE_TOOLCHAIN_H
