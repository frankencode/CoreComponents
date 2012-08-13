#include "Path.hpp"
#include "Pattern.hpp"
#include "Process.hpp"
#include "ProcessFactory.hpp"
#include "GccCompiler.hpp"

namespace ftl
{

GccCompiler::GccCompiler(int options, String execPath)
	: MachCompiler(options)
{
	if (execPath == "") execPath = Path::lookup("g++");
	String machine = Process::start(execPath + " -dumpmachine")->output()->readLine();
	init(execPath, machine);
}

Ref<MachObject, Owner> GccCompiler::analyse(String source)
{
	String text = Process::start(execPath() + " -MM -MG " + source)->rawOutput()->readAll();
	Ref<StringList, Owner> parts = text->split(Pattern("[:\\\\\n\r ]{1,}"));
	return newInstance<MachObject>(parts->pop(0), parts);
}

bool GccCompiler::compile(Ref<MachObject, Owner> object)
{
	return false;
}

bool GccCompiler::link(Ref<MachObjectList> objectList)
{
	return false;
}

} // namespace ftl
