/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/ProcessFactory>
#include <flux/User>

using namespace flux;
using namespace flux::testing;

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

int echo(int argc, char **argv)
{
    Ref<StringList> commandLine = StringList::create();
    for (int i = 0; i < argc; ++i)
        commandLine->append(argv[i]);
    ferr("Process::cwd() = \"%%\"\n") << Process::cwd();
    ferr("Process::env(\"Hello\") = \"%%\"\n") << Process::env("Hello");
    ferr("commandLine = \"%%\"\n") << commandLine->join(" ");

    Ref<LineSource> source = LineSource::open(stdIn());
    while (true) {
        String line = source->readLine();
        if ((line == "") || (line == "exit"))
            break;
        fout() << line << nl;
    }

    return 13;
}

class WorkerClone: public TestCase
{
    void run()
    {
        Ref<ProcessFactory> factory = TestFactory::create();
        Ref<Process> worker = factory->produce();
        int ret = worker->wait();
        fout("ret = %%\n") << ret;
        fout("\n");

        FLUX_VERIFY(ret == 7);
    }
};

class HelloEcho: public TestCase
{
    void run()
    {
        String execPath = testSuite()->execPath();

        Ref<ProcessFactory> factory = ProcessFactory::create();
        factory->setExecPath(execPath);
        factory->setIoPolicy(Process::ForwardInput | Process::ForwardOutput);
        factory->setArguments(StringList::create() << execPath << "--echo 123");
        factory->setEnvMap(Process::envMap() << EnvMap::Item("Hello", "World!"));

        Ref<Process> process = factory->produce();

        fout("Created child process with pid = %%\n") << process->id();

        process->in()->write("Hello, echo!\nexit\n");
        fout() << "\"" << process->out()->readAll() << "\"" << nl;

        fout("Waiting for child process to finish...\n");

        int ret = process->wait();
        fout("ret = %%\n") << ret;
        fout("\n");

        FLUX_VERIFY(ret == 13);
    }
};

class CurrentProcess: public TestCase
{
    void run()
    {
        fout("Process::cwd() = %%\n") << Process::cwd();
        fout("Process::execPath() = %%\n") << Process::execPath();
        fout("Process::isSuperUser() = %%\n") << Process::isSuperUser();
    }
};


int main(int argc, char **argv)
{
    if (argc == 2)
        return echo(argc, argv);

    FLUX_TESTSUITE_ADD(WorkerClone);
    FLUX_TESTSUITE_ADD(HelloEcho);
    FLUX_TESTSUITE_ADD(CurrentProcess);

    return testSuite()->run(argc, argv);
}
