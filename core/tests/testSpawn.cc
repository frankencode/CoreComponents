/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/Spawn>
#include <cc/InputPipe>
#include <cc/OutputPipe>

using namespace cc;
using namespace cc::testing;

static String execPath;

class SimpleSpawnTest: public TestCase
{
    void run() override
    {
        int ret = Spawn::stage(execPath + " test")->start()->wait();
        CC_VERIFY(ret == 7);
    }
};

class InputOutputPipeTest: public TestCase
{
    void run() override
    {
        auto inPipe = InputPipe::create();
        auto outPipe = OutputPipe::create();
        auto spawn = Spawn::stage(execPath + " echo")
            ->setInputChannel(inPipe)
            ->setOutputChannel(outPipe)
            ->start();
        String message = "Hello, echo!";
        inPipe->write(message);
        inPipe->close();
        String reply = outPipe->readAll();
        CC_INSPECT(message);
        CC_INSPECT(reply);
        CC_VERIFY(message == reply);
        int ret = spawn->wait();
        CC_VERIFY(ret == 11);
    }
};

int main(int argc, char **argv)
{
    execPath = argv[0];
    if (argc == 2) {
        if (String{argv[1]} == "test") {
            fout() << "Hello, echo!" << nl;
            return 7;
        }
        if (String{argv[1]} == "echo") {
            stdIn()->transferTo(stdOut());
            CC_DEBUG;
            return 11;
        }
    }

    CC_TESTSUITE_ADD(SimpleSpawnTest);
    CC_TESTSUITE_ADD(InputOutputPipeTest);

    return TestSuite::instance()->run(argc, argv);
}
