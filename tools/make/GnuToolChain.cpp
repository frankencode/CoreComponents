/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/stdio.h>
#include <fkit/Pattern.h>
#include <fkit/File.h>
#include <fkit/Process.h>
#include <fkit/ProcessFactory.h>
#include "BuildPlan.h"
#include "GnuToolChain.h"

namespace fmake
{

GnuToolChain::GnuToolChain(String execPath)
	: ToolChain(execPath, Process::start(machineCommand(execPath), Process::ForwardOutput)->lineOut()->readLine())
{}

String GnuToolChain::machineCommand(String execPath)
{
	return execPath + " -dumpmachine";
}

String GnuToolChain::machineCommand() const
{
	return machineCommand(execPath());
}

int GnuToolChain::defaultSpeedOptimizationLevel() const
{
	return 2;
}

int GnuToolChain::defaultSizeOptimizationLevel() const
{
	return 1;
}

String GnuToolChain::analyseCommand(BuildPlan *buildPlan, String source) const
{
	Format args;
	args << execPath();
	appendCompileOptions(args, buildPlan);
	args << "-MM" << "-MG" << source;
	return args->join(" ");
}

Ref<Job> GnuToolChain::createAnalyseJob(BuildPlan *buildPlan, String source)
{
	return Job::create(analyseCommand(buildPlan, source));
}

Ref<Module> GnuToolChain::finishAnalyseJob(BuildPlan *buildPlan, Job *job)
{
	Ref<StringList> parts = job->outputText()->split(Pattern("[:\\\\\n\r ]{1,}"));
	return Module::create(job->command(), buildPlan->modulePath(parts->pop(0)), parts, true);
}

Ref<Job> GnuToolChain::createCompileJob(BuildPlan *buildPlan, Module *module)
{
	Format args;
	args << execPath();
	appendCompileOptions(args, buildPlan);
	args << "-c" << "-o" << module->modulePath();
	args << module->sourcePath();
	String command = args->join(" ");
	return Job::create(command);
}

Ref<Job> GnuToolChain::createLinkJob(BuildPlan *buildPlan, Module *module)
{
	Format args;
	args << execPath();
	if (buildPlan->options() & BuildPlan::Static) args << "-static";
	args << "-pthread";
	args << "-o" << module->toolName();
	args << module->modulePath();
	appendLinkOptions(args, buildPlan);
	String command = args->join(" ");
	return Job::create(command);
}

Ref<Job> GnuToolChain::createTestJob(BuildPlan *buildPlan, Module *module)
{
	return Job::create(module->toolName());
}

String GnuToolChain::linkName(BuildPlan *buildPlan) const
{
	String path;
	if (buildPlan->options() & BuildPlan::Library)
		path = "lib" + buildPlan->name() + ".so." + buildPlan->version();
	else
		path =  buildPlan->name();
	return path;
}

bool GnuToolChain::link(BuildPlan *buildPlan)
{
	String name = buildPlan->name();
	String version = buildPlan->version();
	int options = buildPlan->options();
	ModuleList *modules = buildPlan->modules();

	Format args;

	args << execPath();
	if (options & BuildPlan::Static) args << "-static";
	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static)) args << "-shared";
	args << "-pthread";

	if (options & BuildPlan::Library) {
		Ref<StringList> versions = version->split(".");
		args << "-Wl,-soname,lib" + name + ".so." + versions->at(0);
	}

	args << "-o" << linkName(buildPlan);

	for (int i = 0; i < modules->size(); ++i)
		args << modules->at(i)->modulePath();

	appendLinkOptions(args, buildPlan);

	String command = args->join(" ");

	if (!buildPlan->runBuild(command))
		return false;

	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static))
		createLibrarySymlinks(buildPlan, linkName(buildPlan));

	return true;
}

bool GnuToolChain::install(BuildPlan *buildPlan)
{
	String product = linkName(buildPlan);
	int options = buildPlan->options();
	String installPath = buildPlan->installPath(
		((options & BuildPlan::Library) ? "lib/" : "bin/") + product
	);
	if (!buildPlan->install(product, installPath)) return false;
	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static))
		createLibrarySymlinks(buildPlan, installPath);
	return true;
}

bool GnuToolChain::install(BuildPlan *buildPlan, Module *module)
{
	String product = module->toolName();
	return buildPlan->install(product, buildPlan->installPath("bin/" + product));
}

bool GnuToolChain::uninstall(BuildPlan *buildPlan)
{
	String product = linkName(buildPlan);
	int options = buildPlan->options();
	String installPath = buildPlan->installPath(
		((options & BuildPlan::Library) ? "lib/" : "bin/") + product
	);
	if (!buildPlan->unlink(installPath)) return false;
	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static))
		cleanLibrarySymlinks(buildPlan, installPath);
	return false;
}

bool GnuToolChain::uninstall(BuildPlan *buildPlan, Module *module)
{
	return buildPlan->unlink(buildPlan->installPath("bin/" + module->toolName()));
}

void GnuToolChain::clean(BuildPlan *buildPlan)
{
	for (int i = 0; i < buildPlan->modules()->size(); ++i) {
		buildPlan->unlink(buildPlan->modules()->at(i)->modulePath());
		if (buildPlan->options() & BuildPlan::Tools)
			buildPlan->unlink(buildPlan->modules()->at(i)->toolName());
	}

	String product = linkName(buildPlan);
	buildPlan->unlink(product);

	if ((buildPlan->options() & BuildPlan::Library) && !(buildPlan->options() & BuildPlan::Static))
		cleanLibrarySymlinks(buildPlan, product);
}

void GnuToolChain::appendCompileOptions(Format args, BuildPlan *buildPlan)
{
	// args << "-std=c++0x";
	if (buildPlan->options() & BuildPlan::Debug) args << "-g";
	if (buildPlan->options() & BuildPlan::Release) args << "-DNDEBUG";
	if (buildPlan->options() & BuildPlan::OptimizeSpeed) args << String(Format("-O") << buildPlan->speedOptimizationLevel());
	if (buildPlan->options() & BuildPlan::OptimizeSize) args << "-Os";
	if (buildPlan->options() & BuildPlan::OptimizeDebug) args << "-Og";
	if (buildPlan->options() & BuildPlan::Static) args << "-static";
	if (buildPlan->options() & BuildPlan::Library) args << "-fpic";
	args << "-Wall" << "-pthread";
	for (int i = 0; i < buildPlan->includePaths()->size(); ++i)
		args << "-I" + buildPlan->includePaths()->at(i);
}

void GnuToolChain::appendLinkOptions(Format args, BuildPlan *buildPlan)
{
	StringList *libraryPaths = buildPlan->libraryPaths();
	StringList *libraries = buildPlan->libraries();

	for (int i = 0; i < libraryPaths->size(); ++i)
		args << "-L" + libraryPaths->at(i);

	for (int i = 0; i < libraries->size(); ++i)
		args << "-l" + libraries->at(i);

	if (libraryPaths->size() > 0) {
		Ref<StringList> rpaths = StringList::create();
		for (int i = 0; i < libraryPaths->size(); ++i)
			*rpaths << "-rpath=" + libraryPaths->at(i)->absolutePath();
		args << "-Wl,--enable-new-dtags," + rpaths->join(",");
	}
}

void GnuToolChain::createLibrarySymlinks(BuildPlan *buildPlan, String libPath)
{
	cleanLibrarySymlinks(buildPlan, libPath);

	Ref<StringList> parts = libPath->split('.');
	while (parts->popBack() != "so")
		buildPlan->symlink(libPath, parts->join("."));
}

void GnuToolChain::cleanLibrarySymlinks(BuildPlan *buildPlan, String libPath)
{
	Ref<StringList> parts = libPath->split('.');
	while (parts->popBack() != "so")
		buildPlan->unlink(parts->join("."));
}

} // namespace fmake
