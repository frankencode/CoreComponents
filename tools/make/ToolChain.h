/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXMAKE_TOOLCHAIN_H
#define FLUXMAKE_TOOLCHAIN_H

#include "Job.h"
#include "Module.h"

namespace fluxmake
{

using namespace flux;

class BuildPlan;
class DependencyCache;

class ToolChain: public Object
{
public:
	inline String compiler() const { return compiler_; }
	inline String machine() const { return machine_; }

	virtual String machineCommand() const = 0;
	virtual String defaultOptimization(BuildPlan *plan) const = 0;

	virtual String analyseCommand(BuildPlan *plan, String source) const = 0;
	virtual Ref<Job> createAnalyseJob(BuildPlan *plan, String source) = 0;
	virtual Ref<Module> finishAnalyseJob(BuildPlan *plan, Job *job) = 0;

	virtual Ref<Job> createCompileJob(BuildPlan *plan, Module *module) = 0;
	virtual Ref<Job> createLinkJob(BuildPlan *plan, Module *module) = 0;
	virtual Ref<Job> createTestJob(BuildPlan *plan, Module *module) = 0;

	virtual String linkName(BuildPlan *plan) const = 0;
	virtual bool link(BuildPlan *plan) = 0;
	virtual bool linkTest(BuildPlan *plan, String linkPath, StringList *linkTest) const = 0;

	virtual bool install(BuildPlan *plan) = 0;
	virtual bool install(BuildPlan *plan, Module *module) = 0;
	virtual bool uninstall(BuildPlan *plan) = 0;
	virtual bool uninstall(BuildPlan *plan, Module *module) = 0;

	virtual void clean(BuildPlan *plan) = 0;

protected:
	ToolChain(String compiler, String machine)
		: compiler_(compiler),
		  machine_(machine)
	{}

private:
	String compiler_;
	String machine_;
};

} // namespace fluxmake

#endif // FLUXMAKE_TOOLCHAIN_H
