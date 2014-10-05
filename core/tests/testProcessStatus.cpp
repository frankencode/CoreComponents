/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

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
	fout("status.loginName() = \"%%\"\n") << status.loginName();
	fout("status.commandName() = \"%%\"\n") << status.commandName();
	return 0;
}
