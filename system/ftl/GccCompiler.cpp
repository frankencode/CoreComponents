#include "Path.hpp"
#include "Format.hpp"
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
	return 0;
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
