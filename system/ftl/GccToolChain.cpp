#include "StandardStreams.hpp"
#include "Pattern.hpp"
#include "File.hpp"
#include "Process.hpp"
#include "ProcessFactory.hpp"
#include "SymbolicLink.hpp"
#include "GccToolChain.hpp"

namespace ftl
{

GccToolChain::GccToolChain(String execPath)
{
	String machine = Process::start(execPath + " -dumpmachine", Process::ForwardOutput)->output()->readLine();
	init(execPath, machine);
}

String GccToolChain::analyseCommand(String source, int options) const
{
	Format args;
	appendCompileOptions(args, options);
	args << "-MM" << "-MG" << source;
	return args->join(" ");
}

Ref<Module, Owner> GccToolChain::analyse(String source, int options)
{
	String command = analyseCommand(source, options);
	output()->writeLine(command);
	String text = Process::start(command, Process::ForwardOutput)->rawOutput()->readAll();
	Ref<StringList, Owner> parts = text->split(Pattern("[:\\\\\n\r ]{1,}"));
	return Module::newInstance(command, parts->pop(0), parts, true);
}

bool GccToolChain::compile(Ref<Module, Owner> module, int options)
{
	Format args;
	appendCompileOptions(args, options, module->modulePath());
	args << module->sourcePath();

	String command = args->join(" ");
	output()->writeLine(command);

	return Process::start(command)->wait() == 0;
}

String GccToolChain::linkPath(String name, String version, int options) const
{
	String path;
	if (options & Library)
		path = "lib" + name + ".so." + version;
	return path;
}

bool GccToolChain::link(Ref<ModuleList> modules, Ref<StringList> libraries, String name, String version, int options)
{
	Format args;

	args << execPath();
	if (options & Static) args << "-static";
	if ((options & Library) && (!(options & Static))) args << "-shared";

	if (options & Library) {
		Ref<StringList, Owner> versions = version->split(".");
		args << "-Wl,-soname,lib" + name + ".so." + versions->at(0);
		args << "-o" << linkPath(name, version, options) ;
	}

	for (int i = 0; i < modules->length(); ++i)
		args << modules->at(i)->modulePath();

	for (int i = 0; i < libraries->length(); ++i)
		args << "-l" + libraries->at(i);

	String command = args->join(" ");
	output()->writeLine(command);

	if (Process::start(command)->wait() != 0)
		return false;

	if (options & Library) {
		String fullPath = linkPath(name, version, options);
		Ref<StringList, Owner> parts = fullPath->split('.');
		while (parts->popBack() != "so")
			SymbolicLink::create(fullPath, parts->join("."));
	}

	return true;
}

void GccToolChain::appendCompileOptions(Format args, int options, String modulePath) const
{
	args << execPath();
	if (options & Debug) args << "-g";
	if (options & OptimizeSpeed) args << "-O3";
	if (options & OptimizeSize) args << "-Os";
	if (options & Static) args << "-static";
	if (options & Library) args << "-fpic";
	if (modulePath != "") {
		args << "-c";
		args << "-o" << modulePath;
	}
}

} // namespace ftl
