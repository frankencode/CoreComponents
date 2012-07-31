#include "WireObject.hpp"
#include "MachCompiler.hpp"

namespace ftl
{

MachCompiler::MachCompiler(int options)
	: options_(options)
{}

bool MachCompiler::build(Ref<WireObject> recipe)
{
	return true;
}

void MachCompiler::init(String execPath, String machine)
{
	execPath_ = execPath;
	machine_ = machine;
}

} // namespace ftl
