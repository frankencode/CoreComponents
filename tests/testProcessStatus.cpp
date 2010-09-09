#include <ftl/stdio>
#include <ftl/process>

namespace ftl
{

int main(int argc, char** argv)
{
	int pid = Process::currentProcessId();
	ProcessStatus status(pid);
	print("status.processId() = %%\n", status.processId());
	print("status.parentProcessId() = %%\n", status.parentProcessId());
	print("status.processGroupId() = %%\n", status.processGroupId());
	print("status.foregroundProcessGroupId() = %%\n", status.foregroundProcessGroupId());
	// print("status.terminalName() = \"%%\"\n", status.terminalName());
	print("status.loginName() = \"%%\"\n", status.loginName());
	print("status.commandName() = \"%%\"\n", status.commandName());
	print("status.processStatus() = %%\n", status.processStatus());
	return 0;
}

} // namespace ftl

int main(int argc, char** argv)
{
	return ftl::main(argc, argv);
}
