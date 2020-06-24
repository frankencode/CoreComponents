/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/Channel>
#include <cc/Worker>

using namespace cc;
using namespace cc::testing;

class WorkerTest: public TestCase
{
    void run() override
    {
        const int n = 20;

        Channel<int> channel;

        Worker producer{[=]{
            Channel<int> out = channel;
            for (int i = 0; i < n; ++i)
                out->pushBack(i);
        }};

        Worker consumer{[=]{
            Channel<int> in = channel;
            for (int i = 0; i < n; ++i)
                fout() << in->popFront() << nl;
        }};
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(WorkerTest);

    return TestSuite::instance()->run(argc, argv);
}
