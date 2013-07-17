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

	virtual String analyseCommand(BuildPlan *buildPlan, String source) const;
	virtual Ref<Job> createAnalyseJob(BuildPlan *buildPlan, String source);
	virtual Ref<Module> finishAnalyseJob(BuildPlan *buildPlan, Job *job);

	virtual Ref<Job> createCompileJob(BuildPlan *buildPlan, Module *module);
	virtual Ref<Job> createLinkJob(BuildPlan *buildPlan, Module *module);
	virtual Ref<Job> createTestJob(BuildPlan *buildPlan, Module *module);

	virtual String linkName(BuildPlan *buildPlan) const;
	virtual bool link(BuildPlan *buildPlan);

	virtual bool install(BuildPlan *buildPlan);
	virtual bool install(BuildPlan *buildPlan, Module *module);
	virtual bool uninstall(BuildPlan *buildPlan);
	virtual bool uninstall(BuildPlan *buildPlan, Module *module);

	virtual void clean(BuildPlan *buildPlan);

protected:
	GnuToolChain(String execPath);
	static String machineCommand(String execPath);

	static void appendCompileOptions(Format args, BuildPlan *buildPlan);
	static void appendLinkOptions(Format args, BuildPlan *buildPlan);

	static void createLibrarySymlinks(BuildPlan *buildPlan, String libPath);
	static void cleanLibrarySymlinks(BuildPlan *buildPlan, String libPath);
};

} // namespace fmake

#endif // FMAKE_GNUTOOLCHAIN_H
