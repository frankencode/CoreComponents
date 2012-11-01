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
	: ToolChain(execPath, Process::start(execPath + " -dumpmachine", Process::ForwardOutput)->output()->readLine())
{}

String GccToolChain::analyseCommand(String source, int options, Ref<StringList> includePaths) const
{
	Format args;
	appendCompileOptions(args, options, includePaths);
	args << "-MM" << "-MG" << source;
	return args->join(" ");
}

Ref<Module, Owner> GccToolChain::analyse(Ref<BuildLine> buildLine, String source, int options, Ref<StringList> includePaths)
{
	String command = analyseCommand(source, options, includePaths);
	String text = buildLine->runAnalyse(command);
	Ref<StringList, Owner> parts = text->split(Pattern("[:\\\\\n\r ]{1,}"));
	return Module::newInstance(command, parts->pop(0), parts, true);
}

bool GccToolChain::compile(Ref<BuildLine> buildLine, Ref<Module, Owner> module, int options, Ref<StringList> includePaths)
{
	Format args;
	appendCompileOptions(args, options, includePaths, module->modulePath());
	args << module->sourcePath();
	String command = args->join(" ");
	return buildLine->runBuild(command);
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
	Ref<BuildLine> buildLine,
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

	if (libraryPaths->length() > 0) {
		Ref<StringList, Owner> rpaths = StringList::newInstance();
		for (int i = 0; i < libraryPaths->length(); ++i)
			rpaths << "-rpath=" + libraryPaths->at(i);
		args << "-Wl,--enable-new-dtags," + rpaths->join(",");
	}

	String command = args->join(" ");

	if (!buildLine->runBuild(command))
		return false;

	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static)) {
		String fullPath = linkPath(name, version, options);
		Ref<StringList, Owner> parts = fullPath->split('.');
		while (parts->popBack() != "so")
			buildLine->symlink(fullPath, parts->join("."));
	}

	return true;
}

void GccToolChain::clean(Ref<BuildLine> buildLine, Ref<ModuleList> modules, int options)
{
	for (int i = 0; i < modules->length(); ++i)
		buildLine->unlink(modules->at(i)->modulePath());
}

void GccToolChain::distClean(Ref<BuildLine> buildLine, Ref<ModuleList> modules, String name, String version, int options)
{
	clean(buildLine, modules, options);

	String fullPath = linkPath(name, version, options);
	buildLine->unlink(fullPath);

	if ((options & BuildPlan::Library) && !(options & BuildPlan::Static)) {
		Ref<StringList, Owner> parts = fullPath->split('.');
		while (parts->popBack() != "so")
			buildLine->unlink(parts->join("."));
	}
}

void GccToolChain::appendCompileOptions(Format args, int options, Ref<StringList> includePaths, String outputPath) const
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
	if (outputPath != "") {
		if (outputPath->tail(2) == ".o") args << "-c";
		args << "-o" << outputPath;
	}
}

} // namespace ftl
