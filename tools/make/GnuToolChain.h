/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FMAKE_GNUTOOLCHAIN_H
#define FMAKE_GNUTOOLCHAIN_H

#include <fkit/Format.h>
#include "ToolChain.h"

namespace fmake
{

using namespace fkit;

class GnuToolChain: public ToolChain
{
public:
	inline static Ref<GnuToolChain> create(String execPath = "g++") {
		return new GnuToolChain(execPath);
	}

	virtual String machineCommand() const;
	virtual int defaultSpeedOptimizationLevel() const;
	virtual int defaultSizeOptimizationLevel() const;

	virtual String analyseCommand(BuildPlan *plan, String source) const;
	virtual Ref<Job> createAnalyseJob(BuildPlan *plan, String source);
	virtual Ref<Module> finishAnalyseJob(BuildPlan *plan, Job *job);

	virtual Ref<Job> createCompileJob(BuildPlan *plan, Module *module);
	virtual Ref<Job> createLinkJob(BuildPlan *plan, Module *module);
	virtual Ref<Job> createTestJob(BuildPlan *plan, Module *module);

	virtual String linkName(BuildPlan *plan) const;
	virtual bool link(BuildPlan *plan);

	virtual bool install(BuildPlan *plan);
	virtual bool install(BuildPlan *plan, Module *module);
	virtual bool uninstall(BuildPlan *plan);
	virtual bool uninstall(BuildPlan *plan, Module *module);

	virtual void clean(BuildPlan *plan);

protected:
	GnuToolChain(String execPath);
	static String machineCommand(String execPath);

	static void appendCompileOptions(Format args, BuildPlan *plan);
	static void appendLinkOptions(Format args, BuildPlan *plan);

	static void createLibrarySymlinks(BuildPlan *plan, String libPath);
	static void cleanLibrarySymlinks(BuildPlan *plan, String libPath);
};

} // namespace fmake

#endif // FMAKE_GNUTOOLCHAIN_H
