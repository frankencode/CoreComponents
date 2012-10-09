#include "StandardStreams.hpp"
#include "Pattern.hpp"
#include "File.hpp"
#include "Process.hpp"
#include "ProcessFactory.hpp"
#include "File.hpp"
#include "GccToolChain.hpp"

namespace ftl
{

GccToolChain::GccToolChain(String execPath)
{
	String machine = Process::start(execPath + " -dumpmachine", Process::ForwardOutput)->output()->readLine();
	init(execPath, machine);
}

String GccToolChain::analyseCommand(String source, int options, Ref<StringList> includePaths) const
{
	Format args;
	appendCompileOptions(args, options, includePaths);
	args << "-MM" << "-MG" << source;
	return args->join(" ");
}

Ref<Module, Owner> GccToolChain::analyse(String source, int options, Ref<StringList> includePaths)
{
	String command = analyseCommand(source, options, includePaths);
	output()->writeLine(command);
	String text = Process::start(command, Process::ForwardOutput)->rawOutput()->readAll();
	Ref<StringList, Owner> parts = text->split(Pattern("[:\\\\\n\r ]{1,}"));
	return Module::newInstance(command, parts->pop(0), parts, true);
}

bool GccToolChain::compile(Ref<Module, Owner> module, int options, Ref<StringList> includePaths)
{
	Format args;
	appendCompileOptions(args, options, includePaths, module->modulePath());
	args << module->sourcePath();

	String command = args->join(" ");
	output()->writeLine(command);

	return Process::start(command)->wait() == 0;
}

String GccToolChain::linkPath(String name, String version, int options) const
{
	String path;
	if (options & BuildPlan::Library)
		path = "lib" + name + ".so." + version;
	else
		path = name;
	return path;
}

bool GccToolChain::link(
	Ref<ModuleList> modules,
	Ref<StringList> libraryPaths,
	Ref<StringList> libraries,
	String name,
	String version,
	int options
)
{
	Format args;

	args << execPath();
	if (options & BuildPlan::Static) args << "-static";
	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static)) args << "-shared";
	args << "-pthread";

	if (options & BuildPlan::Library) {
		Ref<StringList, Owner> versions = version->split(".");
		args << "-Wl,-soname,lib" + name + ".so." + versions->at(0);
	}
	args << "-o" << linkPath(name, version, options) ;

	for (int i = 0; i < modules->length(); ++i)
		args << modules->at(i)->modulePath();

	for (int i = 0; i < libraryPaths->length(); ++i)
		args << "-L" + libraryPaths->at(i);

	for (int i = 0; i < libraries->length(); ++i)
		args << "-l" + libraries->at(i);

	String command = args->join(" ");
	output()->writeLine(command);

	if (Process::start(command)->wait() != 0)
		return false;

	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static)) {
		String fullPath = linkPath(name, version, options);
		Ref<StringList, Owner> parts = fullPath->split('.');
		while (parts->popBack() != "so")
			File::symlink(fullPath, parts->join("."));
	}

	return true;
}

void GccToolChain::clean(Ref<ModuleList> modules, int options)
{
	for (int i = 0; i < modules->length(); ++i)
		unlink(modules->at(i)->modulePath());
}

void GccToolChain::distClean(Ref<ModuleList> modules, String name, String version, int options)
{
	clean(modules, options);

	String fullPath = linkPath(name, version, options);
	unlink(fullPath);

	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static)) {
		Ref<StringList, Owner> parts = fullPath->split('.');
		while (parts->popBack() != "so")
			unlink(parts->join("."));
	}
}

void GccToolChain::appendCompileOptions(Format args, int options, Ref<StringList> includePaths, String modulePath) const
{
	args << execPath();
	if (options & BuildPlan::Debug) args << "-g";
	if (options & BuildPlan::OptimizeSpeed) args << "-O3";
	if (options & BuildPlan::OptimizeSize) args << "-Os";
	if (options & BuildPlan::Static) args << "-static";
	if (options & BuildPlan::Library) args << "-fpic";
	args << "-Wall" << "-pthread";
	for (int i = 0; i < includePaths->length(); ++i)
		args << "-I" + includePaths->at(i);
	if (modulePath != "") {
		args << "-c";
		args << "-o" << modulePath;
	}
}

} // namespace ftl
