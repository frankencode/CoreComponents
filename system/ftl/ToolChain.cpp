#include "PrintDebug.hpp"
#include "File.hpp"
#include "FileStatus.hpp"
#include "ToolChain.hpp"

namespace ftl
{

ToolChain::ToolChain()
{}

void ToolChain::init(String execPath, String machine)
{
	execPath_ = execPath;
	machine_ = machine;
}

bool ToolChain::unlink(const char *path)
{
	if (FileStatus::newInstance(path)->exists()) {
		print("unlink %%\n", path);
		return File::unlink(path);
	}
	return false;
}

} // namespace ftl
