#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/Worker>
#include <cc/can/VirtualCanBus>

using namespace cc;
using namespace cc::can;
using namespace cc::testing;

class SimpleReadWriteTest: public TestCase
{
    void run() override
    {
        CC_INSPECT(sizeof(CanFrame));

        auto bus = VirtualCanBus::create();

        CanFrame inFrame{0x123};
        {
            for (int i = 0; i < inFrame->payloadCount(); ++i)
                inFrame->payloadAt(i) = i + 1;
        }
        CC_INSPECT(inFrame);

        auto media1 = bus->connect();
        auto media2 = bus->connect();

        media1->writeFrame(inFrame);
        CC_VERIFY(media2->waitFrame());

        CanFrame outFrame;
        media2->readFrame(&outFrame);

        CC_INSPECT(outFrame);
        CC_VERIFY(inFrame == outFrame);

        bus->shutdown();
    }
};

class ComplexReadWriteTest: public TestCase
{
    void run() override
    {
        const int n = 10; // number of frame to generate (<= 255)

        auto bus = VirtualCanBus::create();

        // log messages and verify that all messages are transmitted
        auto monitorMedia = bus->connect();
        auto monitor = Worker::start([=]{
            int m = 0;
            for (CanFrame frame; monitorMedia->readFrame(&frame);) {
                ++m;
                fout("(%%) %%\n") << m << frame;
                if (frame->canId() == 0xCBA && frame->payloadAt(0) == n) break;
            }
            bus->shutdown();
            CC_VERIFY(m == 2 * n);
        });

        // echo all messages
        auto echoMedia = bus->connect();
        auto echo = Worker::start([=]{
            for (CanFrame frame; echoMedia->readFrame(&frame);) {
                frame->setCanId(0xCBA);
                echoMedia->writeFrame(frame);
            }
        });

        // generate n messages
        auto generatorMedia = bus->connect();
        auto generator = Worker::start([=]{
            CanFrame frame{0xABC};
            for (int i = 0; i < n; ++i) {
                for (int k = 0; k < frame->payloadCount(); ++k)
                    frame->payloadAt(k) = i + k + 1;
                generatorMedia->writeFrame(frame);
            }
        });
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimpleReadWriteTest);
    CC_TESTSUITE_ADD(ComplexReadWriteTest);

    return TestSuite::instance()->run(argc, argv);
}
