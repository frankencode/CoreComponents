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

        auto channel = Channel<int>::create();

        auto consumer = Worker::start([=]{
            for (int i = 0; i < n; ++i)
                fout() << channel->popFront() << nl;
        });

        auto producer = Worker::start([=]{
            for (int i = 0; i < n; ++i)
                channel->pushBack(i);
        });
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(WorkerTest);

    return TestSuite::instance()->run(argc, argv);
}
