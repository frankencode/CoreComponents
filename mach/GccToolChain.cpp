#include <ftl/StandardStreams.hpp>
#include <ftl/Pattern.hpp>
#include <ftl/Format.hpp>
#include <ftl/File.hpp>
#include <ftl/Process.hpp>
#include <ftl/ProcessFactory.hpp>
#include "BuildPlan.hpp"
#include "GccToolChain.hpp"

namespace mach
{

GccToolChain::GccToolChain(String execPath)
	: ToolChain(execPath, Process::start(machineCommand(execPath), Process::ForwardOutput)->output()->readLine())
{}

String GccToolChain::machineCommand(String execPath)
{
	return execPath + " -dumpmachine";
}

String GccToolChain::machineCommand() const
{
	return machineCommand(execPath());
}

int GccToolChain::defaultSpeedOptimizationLevel() const
{
	return 2;
}

int GccToolChain::defaultSizeOptimizationLevel() const
{
	return 1;
}

String GccToolChain::analyseCommand(Ref<BuildPlan> buildPlan, String source) const
{
	Format args;
	args << execPath();
	appendCompileOptions(args, buildPlan);
	args << "-MM" << "-MG" << source;
	return args->join(" ");
}

Ref<Job, Owner> GccToolChain::createAnalyseJob(Ref<BuildPlan> buildPlan, String source)
{
	return Job::create(analyseCommand(buildPlan, source));
}

Ref<Module, Owner> GccToolChain::finishAnalyseJob(Ref<BuildPlan> buildPlan, Ref<Job> job)
{
	Ref<StringList, Owner> parts = job->outputText()->split(Pattern("[:\\\\\n\r ]{1,}"));
	return Module::create(job->command(), buildPlan->modulePath(parts->pop(0)), parts, true);
}

Ref<Job, Owner> GccToolChain::createCompileJob(Ref<BuildPlan> buildPlan, Ref<Module> module)
{
	Format args;
	args << execPath();
	appendCompileOptions(args, buildPlan);
	args << "-c" << "-o" << module->modulePath();
	args << module->sourcePath();
	String command = args->join(" ");
	return Job::create(command);
}

Ref<Job, Owner> GccToolChain::createLinkJob(Ref<BuildPlan> buildPlan, Ref<Module> module)
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

String GccToolChain::linkPath(Ref<BuildPlan> buildPlan) const
{
	String path;
	if (buildPlan->options() & BuildPlan::Library)
		path = "lib" + buildPlan->name() + ".so." + buildPlan->version();
	else
		path =  buildPlan->name();
	return path;
}

bool GccToolChain::link(Ref<BuildPlan> buildPlan)
{
	String name = buildPlan->name();
	String version = buildPlan->version();
	int options = buildPlan->options();
	Ref<ModuleList> modules = buildPlan->modules();

	Format args;

	args << execPath();
	if (options & BuildPlan::Static) args << "-static";
	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static)) args << "-shared";
	args << "-pthread";

	if (options & BuildPlan::Library) {
		Ref<StringList, Owner> versions = version->split(".");
		args << "-Wl,-soname,lib" + name + ".so." + versions->at(0);
	}

	args << "-o" << linkPath(buildPlan);

	for (int i = 0; i < modules->length(); ++i)
		args << modules->at(i)->modulePath();

	appendLinkOptions(args, buildPlan);

	String command = args->join(" ");

	if (!buildPlan->runBuild(command))
		return false;

	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static)) {
		String fullPath = linkPath(buildPlan);
		Ref<StringList, Owner> parts = fullPath->split('.');
		while (parts->popBack() != "so")
			buildPlan->symlink(fullPath, parts->join("."));
	}

	return true;
}

void GccToolChain::clean(Ref<BuildPlan> buildPlan)
{
	for (int i = 0; i < buildPlan->modules()->length(); ++i) {
		buildPlan->unlink(buildPlan->modules()->at(i)->modulePath());
		if (buildPlan->options() & BuildPlan::ToolSet)
			buildPlan->unlink(buildPlan->modules()->at(i)->toolName());
	}

	String fullPath = linkPath(buildPlan);
	buildPlan->unlink(fullPath);

	if ((buildPlan->options() & BuildPlan::Library) && !(buildPlan->options() & BuildPlan::Static)) {
		Ref<StringList, Owner> parts = fullPath->split('.');
		while (parts->popBack() != "so")
			buildPlan->unlink(parts->join("."));
	}
}

void GccToolChain::appendCompileOptions(Format args, Ref<BuildPlan> buildPlan)
{
	args << "-std=c++0x";
	if (buildPlan->options() & BuildPlan::Debug) args << "-g";
	if (buildPlan->options() & BuildPlan::Release) args << "-DNDEBUG";
	if (buildPlan->options() & BuildPlan::OptimizeSpeed) args << String(Format("-O%%") << buildPlan->speedOptimizationLevel());
	if (buildPlan->options() & BuildPlan::OptimizeSize) args << "-Os";
	if (buildPlan->options() & BuildPlan::Static) args << "-static";
	if (buildPlan->options() & BuildPlan::Library) args << "-fpic";
	args << "-Wall" << "-pthread";
	for (int i = 0; i < buildPlan->includePaths()->length(); ++i)
		args << "-I" + buildPlan->includePaths()->at(i);
}

void GccToolChain::appendLinkOptions(Format args, Ref<BuildPlan> buildPlan)
{
	Ref<StringList> libraryPaths = buildPlan->libraryPaths();
	Ref<StringList> libraries = buildPlan->libraries();

	for (int i = 0; i < libraryPaths->length(); ++i)
		args << "-L" + libraryPaths->at(i);

	for (int i = 0; i < libraries->length(); ++i)
		args << "-l" + libraries->at(i);

	if (libraryPaths->length() > 0) {
		Ref<StringList, Owner> rpaths = StringList::create();
		for (int i = 0; i < libraryPaths->length(); ++i)
			rpaths << "-rpath=" + libraryPaths->at(i)->absolutePath();
		args << "-Wl,--enable-new-dtags," + rpaths->join(",");
	}
}

} // namespace mach
