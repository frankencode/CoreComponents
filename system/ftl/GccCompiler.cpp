#include "StandardStreams.hpp"
#include "Pattern.hpp"
#include "File.hpp"
#include "Process.hpp"
#include "ProcessFactory.hpp"
#include "GccCompiler.hpp"

namespace ftl
{

GccCompiler::GccCompiler(String execPath)
{
	String machine = Process::start(execPath + " -dumpmachine", Process::ForwardOutput)->output()->readLine();
	init(execPath, machine);
}

String GccCompiler::analyseCommand(String source, int options) const
{
	Format args;
	appendCompileOptions(args, options);
	args << "-MM" << "-MG" << source;
	return args->join(" ");
}

Ref<MachObject, Owner> GccCompiler::analyse(String source, int options)
{
	String command = analyseCommand(source, options);
	output()->writeLine(command);
	String text = Process::start(command, Process::ForwardOutput)->rawOutput()->readAll();
	Ref<StringList, Owner> parts = text->split(Pattern("[:\\\\\n\r ]{1,}"));
	return MachObject::newInstance(command, parts->pop(0), parts, true);
}

bool GccCompiler::compile(Ref<MachObject, Owner> object, int options)
{
	Format args;
	appendCompileOptions(args, options, object->objectPath());
	args << object->sourcePath();

	String command = args->join(" ");
	output()->writeLine(command);

	return Process::start(command)->wait() == 0;
}

bool GccCompiler::link(Ref<MachObjectList> objects, Ref<StringList> libraries, String name, String version, int options)
{
	Format args;

	args << execPath();
	if (options & Static) args << "-static";
	if ((options & Library) && (!(options & Static))) args << "-shared";

	if (options & Library) {
		Ref<StringList, Owner> versions = version->split(".");
		args << "-Wl,-soname,lib" + name + ".so." + versions->at(0);
		args << "-o" << "lib" + name + ".so." + version;
	}

	for (int i = 0; i < objects->length(); ++i)
		args << objects->at(i)->objectPath();

	for (int i = 0; i < libraries->length(); ++i)
		args << "-l" + libraries->at(i);

	String command = args->join(" ");
	output()->writeLine(command);

	return Process::start(command)->wait() == 0;
}

void GccCompiler::appendCompileOptions(Format args, int options, String objectPath) const
{
	args << execPath();
	if (options & Debug) args << "-g";
	if (options & OptimizeSpeed) args << "-O3";
	if (options & OptimizeSize) args << "-Os";
	if (options & Static) args << "-static";
	if (options & Library) args << "-fpic";
	if (objectPath != "") {
		args << "-c";
		args << "-o" << objectPath;
	}
}

} // namespace ftl
