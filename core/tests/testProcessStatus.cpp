#include <flux/stdio.h>
#include <flux/ProcessStatus.h>
#include <flux/Process.h>

using namespace flux;

int main(int argc, char **argv)
{
	int pid = Process::currentId();
	ProcessStatus status(pid);
	fout("status.processId() = %%\n") << status.processId();
	fout("status.parentProcessId() = %%\n") << status.parentProcessId();
	fout("status.processGroupId() = %%\n") << status.processGroupId();
	fout("status.foregroundProcessGroupId() = %%\n") << status.foregroundProcessGroupId();
	// fout("status.terminalName() = \"%%\"\n") << status.terminalName();
	fout("status.loginName() = \"%%\"\n") << status.loginName();
	fout("status.commandName() = \"%%\"\n") << status.commandName();
	fout("status.processStatus() = %%\n") << status.processStatus();
	return 0;
}
