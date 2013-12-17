/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Pattern.h>
#include <flux/File.h>
#include <flux/Process.h>
#include <flux/ProcessFactory.h>
#include "BuildPlan.h"
#include "GnuToolChain.h"

namespace fluxmake
{

Ref<GnuToolChain> GnuToolChain::create(String compiler)
{
	if (compiler == "") compiler = "gcc";
	return new GnuToolChain(compiler);
}

GnuToolChain::GnuToolChain(String compiler)
	: ToolChain(compiler, queryMachine(compiler))
{}

String GnuToolChain::queryMachine(String compiler)
{
	String machine = Process::start(machineCommand(compiler), Process::ForwardOutput)->readAll();
	machine->trimInsitu();
	machine->replaceInsitu("-pc-", "-"); // workaround for clang/bash
	return machine;
}

String GnuToolChain::machineCommand(String compiler)
{
	return compiler + " -dumpmachine";
}

String GnuToolChain::machineCommand() const
{
	return machineCommand(compiler());
}

String GnuToolChain::defaultOptimization(BuildPlan *plan) const
{
	/*if ((plan->options() & BuildPlan::Debug) && (plan->options() & BuildPlan::Release)) return "g";*/
	if (plan->options() & BuildPlan::Release) return "2";
	else return "";
}

String GnuToolChain::analyseCommand(BuildPlan *plan, String source) const
{
	Format args;
	args << compiler();
	appendCompileOptions(args, plan);
	args << "-MM" << source;
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
	args << compiler();
	appendCompileOptions(args, plan);
	args << "-c" << "-o" << module->modulePath();
	args << module->sourcePath();
	String command = args->join(" ");
	return Job::create(command);
}

Ref<Job> GnuToolChain::createLinkJob(BuildPlan *plan, Module *module)
{
	Format args;
	args << compiler();
	if (plan->linkStatic()) args << "-static";
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

	args << compiler();
	if (plan->linkStatic()) args << "-static";
	if ((options & BuildPlan::Library) && !plan->linkStatic()) args << "-shared";
	args << "-pthread";

	if (options & BuildPlan::Library) {
		Ref<StringList> versions = version->split(".");
		args << "-Wl,-soname,lib" + name + ".so." + versions->at(0);
	}

	args << "-o" << linkName(plan);

	appendLinkOptions(args, plan);

	for (int i = 0; i < modules->size(); ++i)
		args << modules->at(i)->modulePath();

	String command = args->join(" ");

	if (!plan->shell()->run(command))
		return false;

	if ((options & BuildPlan::Library) && !plan->linkStatic())
		createLibrarySymlinks(plan, linkName(plan));

	if (options & BuildPlan::Application)
		createAliasSymlinks(plan, linkName(plan));

	return true;
}

bool GnuToolChain::linkTest(BuildPlan *plan, String linkPath, StringList *linkTest) const
{
	Ref<File> src = File::temp();
	src->unlinkWhenDone();
	src->write("int main() { return 0; }\n");
	src->close();
	Format args;
	args << compiler() << src->path() << "-L" + linkPath;
	for (int i = 0; i < linkTest->size(); ++i) args << "-l" + linkTest->at(i);
	args << "-o" + src->path() + "_";

	String command = args->join(" ");
	return plan->shell()->run(command);
}

class CwdGuard {
public:
	CwdGuard(String dirPath): cwdSaved_(Process::cwd()) { Process::cd(dirPath); }
	~CwdGuard() { Process::cd(cwdSaved_); }
private:
	String cwdSaved_;
};

bool GnuToolChain::install(BuildPlan *plan)
{
	int options = plan->options();
	String product = linkName(plan);
	String installPrefix = plan->installPath((options & BuildPlan::Library) ? "lib" : "bin");
	String installPath = installPrefix->expandPath(product);

	if (!plan->shell()->install(product, installPath)) return false;

	{
		CwdGuard guard(installPrefix);

		if ((options & BuildPlan::Library) && !plan->linkStatic())
			createLibrarySymlinks(plan, product);

		if (options & BuildPlan::Application)
			createAliasSymlinks(plan, product);
	}

	return true;
}

bool GnuToolChain::install(BuildPlan *plan, Module *module)
{
	String product = module->toolName();
	return plan->shell()->install(product, plan->installPath("bin/" + product));
}

bool GnuToolChain::uninstall(BuildPlan *plan)
{
	int options = plan->options();
	String product = linkName(plan);
	String installPrefix = plan->installPath((options & BuildPlan::Library) ? "lib" : "bin");
	String installPath = installPrefix->expandPath(product);

	if (!plan->shell()->unlink(installPath)) return false;

	{
		CwdGuard guard(installPrefix);

		if ((options & BuildPlan::Library) && !plan->linkStatic())
			cleanLibrarySymlinks(plan, product);

		if (options & BuildPlan::Application)
			cleanAliasSymlinks(plan, product);
	}

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

	if ((plan->options() & BuildPlan::Library) && !plan->linkStatic())
		cleanLibrarySymlinks(plan, product);

	if (plan->options() & BuildPlan::Application)
		cleanAliasSymlinks(plan, product);
}

void GnuToolChain::appendCompileOptions(Format args, BuildPlan *plan)
{
	if (plan->options() & BuildPlan::Debug) args << "-g";
	if (plan->options() & BuildPlan::Release) args << "-DNDEBUG";
	if (plan->optimize() != "") args << "-O" + plan->optimize();
	if (plan->linkStatic()) args << "-static";
	if (plan->options() & BuildPlan::Library) args << "-fPIC";
	else args << "-fPIE";
	args << "-Wall" << "-pthread" << "-pipe";
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

	if (plan->containsCPlusPlus()) args << "-lstdc++";

	if (libraryPaths->size() > 0) {
		Ref<StringList> rpaths = StringList::create();
		for (int i = 0; i < libraryPaths->size(); ++i)
			*rpaths << "-rpath=" + libraryPaths->at(i)->absolutePath();
		args << "-Wl,--enable-new-dtags," + rpaths->join(",");
	}
}

void GnuToolChain::createLibrarySymlinks(BuildPlan *plan, String libName)
{
	cleanLibrarySymlinks(plan, libName);

	Ref<StringList> parts = libName->split('.');
	while (parts->popBack() != "so")
		plan->shell()->symlink(libName, parts->join("."));
}

void GnuToolChain::cleanLibrarySymlinks(BuildPlan *plan, String libName)
{
	Ref<StringList> parts = libName->split('.');
	while (parts->popBack() != "so")
		plan->shell()->unlink(parts->join("."));
}

void GnuToolChain::createAliasSymlinks(BuildPlan *plan, String appName)
{
	cleanAliasSymlinks(plan, appName);

	for (int i = 0; i < plan->alias()->size(); ++i)
		plan->shell()->symlink(appName, plan->alias()->at(i));
}

void GnuToolChain::cleanAliasSymlinks(BuildPlan *plan, String appName)
{
	for (int i = 0; i < plan->alias()->size(); ++i)
		plan->shell()->unlink(plan->alias()->at(i));
}

} // namespace fluxmake
