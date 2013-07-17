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

	virtual String analyseCommand(BuildPlan *buildPlan, String source) const = 0;
	virtual Ref<Job> createAnalyseJob(BuildPlan *buildPlan, String source) = 0;
	virtual Ref<Module> finishAnalyseJob(BuildPlan *buildPlan, Job *job) = 0;

	virtual Ref<Job> createCompileJob(BuildPlan *buildPlan, Module *module) = 0;
	virtual Ref<Job> createLinkJob(BuildPlan *buildPlan, Module *module) = 0;
	virtual Ref<Job> createTestJob(BuildPlan *buildPlan, Module *module) = 0;

	virtual String linkName(BuildPlan *buildPlan) const = 0;
	virtual bool link(BuildPlan *buildPlan) = 0;

	virtual bool install(BuildPlan *buildPlan) = 0;
	virtual bool install(BuildPlan *buildPlan, Module *module) = 0;
	virtual bool uninstall(BuildPlan *buildPlan) = 0;
	virtual bool uninstall(BuildPlan *buildPlan, Module *module) = 0;

	virtual void clean(BuildPlan *buildPlan) = 0;

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
