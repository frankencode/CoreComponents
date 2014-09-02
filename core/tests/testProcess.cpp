/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio.h>
#include <flux/check.h>
#include <flux/ProcessFactory.h>
#include <flux/User.h>

using namespace flux;

int echo(int argc, char **argv);

class TestFactory: public ProcessFactory
{
public:
	inline static Ref<TestFactory> create() {
		return new TestFactory;
	}
	int incarnate() {
		fout("Good morning %%.\n") << User::lookup(Process::realUserId())->fullName();
		return 7;
	}
};

int main(int argc, char **argv)
{
	if (argc == 2)
		return echo(argc, argv);

	{
		fout("(1) Worker clone\n\n");

		Ref<ProcessFactory> factory = TestFactory::create();
		Ref<Process> worker = factory->produce();
		int ret = worker->wait();
		fout("ret = %%\n") << ret;
		fout("\n");

		check(ret == 7);
	}

	{
		fout("(2) I/O Redirection, passing of arguments and environment variables\n\n");

		Ref<ProcessFactory> factory = ProcessFactory::create();
		factory->setExecPath(argv[0]);
		factory->setIoPolicy(Process::ForwardInput | Process::ForwardOutput);
		factory->setArguments(StringList::create() << argv[0] << "--echo 123");
		factory->setEnvMap(Process::envMap() << EnvMap::Item("Hello", "World!"));

		Ref<Process> process = factory->produce();

		fout("Created child process with pid = %%\n") << process->id();

		process->in()->write("Hello, echo!\nexit\n");
		fout() << "\"" << process->out()->readAll() << "\"" << nl;

		fout("Waiting for child process to finish...\n");

		int ret = process->wait();
		fout("ret = %%\n") << ret;
		fout("\n");

		check(ret == 13);
	}

	{
		fout("(3) Current process\n\n");
		fout("Process::cwd() = %%\n") << Process::cwd();
		fout("Process::execPath() = %%\n") << Process::execPath();
		fout("Process::isSuperUser() = %%\n") << Process::isSuperUser();
		fout("\n");
	}

	return 0;
}

int echo(int argc, char **argv)
{
	Ref<StringList> commandLine = StringList::create();
	for (int i = 0; i < argc; ++i)
		commandLine->append(argv[i]);
	ferr("Process::cwd() = \"%%\"\n") << Process::cwd();
	ferr("Process::env(\"Hello\") = \"%%\"\n") << Process::env("Hello");
	ferr("commandLine = \"%%\"\n") << commandLine->join(" ");

	while (true) {
		String line = lineInput()->readLine();
		if ((line == "") || (line == "exit"))
			break;
		fout() << line << nl;
	}

	return 13;
}
