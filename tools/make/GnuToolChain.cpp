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
	: ToolChain(execPath, queryMachine(execPath))
{}

String GnuToolChain::queryMachine(String execPath)
{
	return Process::start(machineCommand(execPath), Process::ForwardOutput)->lineOut()->readLine();
}

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

String GnuToolChain::analyseCommand(BuildPlan *plan, String source) const
{
	Format args;
	args << execPath();
	appendCompileOptions(args, plan);
	args << "-MM" << "-MG" << source;
	return args->join(" ");
}

Ref<Job> GnuToolChain::createAnalyseJob(BuildPlan *plan, String source)
{
	return Job::create(analyseCommand(plan, source));
}

Ref<Module> GnuToolChain::finishAnalyseJob(BuildPlan *plan, Job *job)
{
	Ref<StringList> parts = job->outputText()->split(Pattern("[:\\\\\n\r ]{1,}"));
	return Module::create(job->command(), plan->modulePath(parts->pop(0)), parts, true);
}

Ref<Job> GnuToolChain::createCompileJob(BuildPlan *plan, Module *module)
{
	Format args;
	args << execPath();
	appendCompileOptions(args, plan);
	args << "-c" << "-o" << module->modulePath();
	args << module->sourcePath();
	String command = args->join(" ");
	return Job::create(command);
}

Ref<Job> GnuToolChain::createLinkJob(BuildPlan *plan, Module *module)
{
	Format args;
	args << execPath();
	if (plan->options() & BuildPlan::Static) args << "-static";
	args << "-pthread";
	args << "-o" << module->toolName();
	args << module->modulePath();
	appendLinkOptions(args, plan);
	String command = args->join(" ");
	return Job::create(command);
}

Ref<Job> GnuToolChain::createTestJob(BuildPlan *plan, Module *module)
{
	return Job::create(module->toolName());
}

String GnuToolChain::linkName(BuildPlan *plan) const
{
	String name;
	if (plan->options() & BuildPlan::Library)
		name = "lib" + plan->name() + ".so." + plan->version();
	else
		name = plan->name();
	return name;
}

bool GnuToolChain::link(BuildPlan *plan)
{
	String name = plan->name();
	String version = plan->version();
	int options = plan->options();
	ModuleList *modules = plan->modules();

	Format args;

	args << execPath();
	if (options & BuildPlan::Static) args << "-static";
	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static)) args << "-shared";
	args << "-pthread";

	if (options & BuildPlan::Library) {
		Ref<StringList> versions = version->split(".");
		args << "-Wl,-soname,lib" + name + ".so." + versions->at(0);
	}

	args << "-o" << linkName(plan);

	for (int i = 0; i < modules->size(); ++i)
		args << modules->at(i)->modulePath();

	appendLinkOptions(args, plan);

	String command = args->join(" ");

	if (!plan->shell()->run(command))
		return false;

	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static))
		createLibrarySymlinks(plan, linkName(plan));

	return true;
}

bool GnuToolChain::linkTest(BuildPlan *plan, String linkPath, StringList *linkTest) const
{
	Ref<File> src = File::temp();
	src->unlinkWhenDone();
	src->write("int main() { return 0; }\n");
	src->close();
	Format args;
	args << execPath() << src->path() << "-L" + linkPath;
	for (int i = 0; i < linkTest->size(); ++i) args << "-l" + linkTest->at(i);
	args << "-o" + src->path() + "_";

	String command = args->join(" ");
	return plan->shell()->run(command);
}

bool GnuToolChain::install(BuildPlan *plan)
{
	String product = linkName(plan);
	int options = plan->options();
	String installPath = plan->installPath(
		((options & BuildPlan::Library) ? "lib/" : "bin/") + product
	);
	if (!plan->shell()->install(product, installPath)) return false;
	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static))
		createLibrarySymlinks(plan, installPath);
	return true;
}

bool GnuToolChain::install(BuildPlan *plan, Module *module)
{
	String product = module->toolName();
	return plan->shell()->install(product, plan->installPath("bin/" + product));
}

bool GnuToolChain::uninstall(BuildPlan *plan)
{
	String product = linkName(plan);
	int options = plan->options();
	String installPath = plan->installPath(
		((options & BuildPlan::Library) ? "lib/" : "bin/") + product
	);
	if (!plan->shell()->unlink(installPath)) return false;
	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static))
		cleanLibrarySymlinks(plan, installPath);
	return false;
}

bool GnuToolChain::uninstall(BuildPlan *plan, Module *module)
{
	return plan->shell()->unlink(plan->installPath("bin/" + module->toolName()));
}

void GnuToolChain::clean(BuildPlan *plan)
{
	for (int i = 0; i < plan->modules()->size(); ++i) {
		plan->shell()->unlink(plan->modules()->at(i)->modulePath());
		if (plan->options() & BuildPlan::Tools)
			plan->shell()->unlink(plan->modules()->at(i)->toolName());
	}

	String product = linkName(plan);
	plan->shell()->unlink(product);

	if ((plan->options() & BuildPlan::Library) && !(plan->options() & BuildPlan::Static))
		cleanLibrarySymlinks(plan, product);
}

void GnuToolChain::appendCompileOptions(Format args, BuildPlan *plan)
{
	// args << "-std=c++0x";
	if (plan->options() & BuildPlan::Debug) args << "-g";
	if (plan->options() & BuildPlan::Release) args << "-DNDEBUG";
	if (plan->options() & BuildPlan::OptimizeSpeed) args << String(Format("-O") << plan->speedOptimizationLevel());
	if (plan->options() & BuildPlan::OptimizeSize) args << "-Os";
	if (plan->options() & BuildPlan::OptimizeDebug) args << "-Og";
	if (plan->options() & BuildPlan::Static) args << "-static";
	if (plan->options() & BuildPlan::Library) args << "-fPIC";
	else args << "-fPIE";
	args << "-Wall" << "-pthread";
	if (plan->customCompileFlags()) {
		for (int i = 0; i < plan->customCompileFlags()->size(); ++i)
			args << plan->customCompileFlags()->at(i);
	}
	for (int i = 0; i < plan->includePaths()->size(); ++i)
		args << "-I" + plan->includePaths()->at(i);
}

void GnuToolChain::appendLinkOptions(Format args, BuildPlan *plan)
{
	if (plan->customLinkFlags()) {
		for (int i = 0; i < plan->customLinkFlags()->size(); ++i)
			args << plan->customLinkFlags()->at(i);
	}

	StringList *libraryPaths = plan->libraryPaths();
	StringList *libraries = plan->libraries();

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

void GnuToolChain::createLibrarySymlinks(BuildPlan *plan, String libPath)
{
	cleanLibrarySymlinks(plan, libPath);

	Ref<StringList> parts = libPath->split('.');
	while (parts->popBack() != "so")
		plan->shell()->symlink(libPath, parts->join("."));
}

void GnuToolChain::cleanLibrarySymlinks(BuildPlan *plan, String libPath)
{
	Ref<StringList> parts = libPath->split('.');
	while (parts->popBack() != "so")
		plan->shell()->unlink(parts->join("."));
}

} // namespace fmake
