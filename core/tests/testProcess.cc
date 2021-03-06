/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/Process>
#include <cc/Spawn>
#include <cc/DEBUG>
#include <cc/stdio>

using namespace cc;
using namespace cc::testing;

static String execPath;

class SimpleSpawnTest: public TestCase
{
    void run() override
    {
        int ret = Process{execPath + " test"}->wait();
        CC_VERIFY(ret == 7);
    }
};

class SimpleEchoTest: public TestCase
{
    void run() override
    {
        Spawn spawn { execPath + " echo" };
        String message = "Hello, echo!";
        spawn->input()->write(message);
        spawn->input()->close();
        CC_DEBUG;
        String reply = spawn->output()->readAll();
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
            return 11;
        }
    }

    CC_TESTSUITE_ADD(SimpleSpawnTest);
    CC_TESTSUITE_ADD(SimpleEchoTest);

    return TestSuite::instance()->run(argc, argv);
}
