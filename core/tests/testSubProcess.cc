/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/LineSource>
#include <cc/File>
#include <cc/UnlinkGuard>
#include <cc/Process>
#include <cc/SubProcess>

using namespace cc;
using namespace cc::testing;

String execPath;

/** \brief test command line invocation and output capture
  */
class SimpleCommandInvocationTest: public TestCase
{
    void run()
    {
        String command = execPath + " 1 2 3 4";
        auto sub = SubProcess::open(command);
        ferr() << "command = \"" << command << "\"" << nl;
        String output = sub->readAll();
        ferr() << "output = \"" << output << "\"" << nl;
        CC_VERIFY(output->toNumber<int>() == 10);
        bool signalled = false;
        CC_VERIFY(sub->wait(&signalled) == 0);
        CC_VERIFY(!signalled);
    }

public:
    static void sumUp(int argc, char **argv)
    {
        int sum = 0;
        for (int i = 1; i < argc; ++i)
            sum += str(argv[i])->toNumber<int>();
        fout() << sum;
    }
};


/** \brief test worker forking and I/O redirection
  */
class SimpleWorkerTest: public TestCase
{
    class AddWorker: public SubProcess::Worker
    {
    public:
        static Ref<AddWorker> create() { return new AddWorker; }

        virtual int run() {
            auto source = LineSource::open(stdIn());
            for (String line; source->read(&line);) {
                if (line == "" || line == "exit") break;
                auto parts = line->simplify()->trim()->split(' ');
                int sum = 0;
                for (int i = 0; i < parts->count(); ++i)
                    sum += parts->at(i)->toNumber<int>();
                fout() << sum << nl;
            }
            return 0;
        }
    private:
        AddWorker() {}
    };

    void run()
    {
        auto sub = SubProcess::open(AddWorker::create());
        String input =
            "1 2 3 4\n"
            "exit\n";
        sub->write(input);
        String output = sub->readAll();
        ferr() << "input = \"" << input << "\"" << nl;
        ferr() << "output = \"" << output << "\"" << nl;
        CC_VERIFY(output->toNumber<int>() == 10);
        bool signalled = false;
        CC_VERIFY(sub->wait(&signalled) == 0);
        CC_VERIFY(!signalled);
    }
};

/*! \brief test abnormal termination
  */
class AbnormalTerminationTest: public TestCase
{
    void run()
    {
        auto sub = SubProcess::open(execPath + " segfault");
        bool signalled = false;
        int ret = sub->wait(&signalled);
        ferr() << "ret = " << ret << nl;
        CC_VERIFY(ret == SIGSEGV);
        CC_VERIFY(signalled);
    }

public:
    static void segfault()
    {
        char *p = 0;
        *p = 0;
    }
};

/*! \brief test I/O forwarding by a pseudo terminal device
  */
class PtsEchoTest: public TestCase
{
    void run()
    {
        auto sub = SubProcess::stage()
            ->setCommand(execPath + " echo")
            ->setForwarding(SubProcess::ForwardByPts)
            ->open();
        String message = "Hello, world!";
        sub->write(message);
        sub->write("\n");
        String echo = LineSource::open(sub)->readLine();
        ferr() << "echo = \"" << echo << "\"" << nl;
        CC_VERIFY(echo == message);
        bool signalled = false;
        CC_VERIFY(sub->wait(&signalled) == 0);
        CC_VERIFY(!signalled);
    }

public:
    static void echo()
    {
        fout() << LineSource::open(stdIn())->readLine() << nl;
    }
};

class ErrorFilterTest: public TestCase
{
    static const char *errorMessage() { return "something went wrong"; }

    void run()
    {
        auto log = File::openTemp();
        String logPath = log->path();;
        auto guard = UnlinkGuard::create(logPath);
        auto sub = SubProcess::open(execPath + " errors", log);
        bool signalled = false;
        CC_VERIFY(sub->wait(&signalled) == 7);
        CC_VERIFY(!signalled);
        log = 0;
        String error = File::open(logPath)->map()->trim();
        fout() << "error = \"" << error << "\"" << nl;
        CC_VERIFY(error == errorMessage());
    }

public:
    static int generateError()
    {
        ferr() << errorMessage() << nl;
        return 7;
    }
};

class WriteShutdownTest: public TestCase
{
    void run()
    {
        auto sub = SubProcess::open(execPath + " reply");
        sub->write("query something");
        sub->shutdown(SubProcess::WriteShutdown);
        String output = sub->readAll();
        ferr() << "output = \"" << output << "\"" << nl;
        CC_VERIFY(output->count() > 0);
        bool signalled = false;
        CC_VERIFY(sub->wait(&signalled) == 0);
        CC_VERIFY(!signalled);
    }

public:
    static void replySomething()
    {
        String reply = stdIn()->readAll()->replace("query", "reply");
        stdOut()->write(reply);
    }
};

class CommandNotFoundTest: public TestCase
{
    void run()
    {
        try {
            auto sub = SubProcess::open("abrakadabra simsalabim");
            CC_VERIFY(false);
        }
        catch (UsageError &ex) {
            ferr() << ex << nl;
        }
    }
};

int main(int argc, char **argv)
{
    execPath = String(argv[0])->absolutePathRelativeTo(Process::cwd());

    if (argc > 1) {
        String testCommand = argv[1];
        if (testCommand == "segfault") {
            AbnormalTerminationTest::segfault();
            return 0;
        }
        else if (testCommand == "echo") {
            PtsEchoTest::echo();
            return 0;
        }
        else if (testCommand == "errors") {
            return ErrorFilterTest::generateError();
        }
        else if (testCommand == "reply") {
            WriteShutdownTest::replySomething();
            return 0;
        }
        else {
            SimpleCommandInvocationTest::sumUp(argc, argv);
            return 0;
        }
    }

    CC_TESTSUITE_ADD(SimpleCommandInvocationTest);
    CC_TESTSUITE_ADD(SimpleWorkerTest);
    CC_TESTSUITE_ADD(AbnormalTerminationTest);
    CC_TESTSUITE_ADD(PtsEchoTest);
    // CC_TESTSUITE_ADD(ErrorFilterTest); // FIXME, not working with '-test-run'
    CC_TESTSUITE_ADD(WriteShutdownTest);
    CC_TESTSUITE_ADD(CommandNotFoundTest);

    return TestSuite::instance()->run(argc, argv);
}
