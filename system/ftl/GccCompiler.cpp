#include "StandardStreams.hpp"
#include "Pattern.hpp"
#include "File.hpp"
#include "Process.hpp"
#include "ProcessFactory.hpp"
#include "GccCompiler.hpp"

namespace ftl
{

GccCompiler::GccCompiler(int options, String execPath)
	: MachCompiler(options)
{
	String machine = Process::start(execPath + " -dumpmachine", Process::ForwardOutput)->output()->readLine();
	init(execPath, machine);
}

Ref<MachObject, Owner> GccCompiler::analyse(String source)
{
	Format args;
	appendCompileOptions(args);
	args << "-MM" << "-MG" << source;
	String command = args->join(" ");
	output()->writeLine(command);
	String text = Process::start(command, Process::ForwardOutput)->rawOutput()->readAll();
	Ref<StringList, Owner> parts = text->split(Pattern("[:\\\\\n\r ]{1,}"));
	return MachObject::newInstance(parts->pop(0), parts);
}

bool GccCompiler::compile(Ref<MachObject, Owner> object)
{
	Format args;
	appendCompileOptions(args);
	args << "-c";
	args << "-o" << object->objectPath();
	args << object->sourcePath();
	String command = args->join(" ");
	output()->writeLine(command);
	return Process::start(command)->wait() == 0;
}

bool GccCompiler::link(Ref<MachObjectList> objectList)
{
	return false;
}

void GccCompiler::appendCompileOptions(Format args)
{
	args << execPath();
	if (options() & DebugInfo) args << "-g";
	if (options() & OptimizeSpeed) args << "-O3";
	if (options() & OptimizeSize) args << "-Os";
	if (options() & SharedLibrary) args << "-shared -fpic";
}

} // namespace ftl
