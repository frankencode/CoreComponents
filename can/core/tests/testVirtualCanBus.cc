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
        auto bus = VirtualCanBus::create();

        auto inFrame = CanFrame::create();
        {
            inFrame->setCanId(0x123);
            for (int i = 0; i < inFrame->payloadCount(); ++i)
                inFrame->payloadAt(i) = i + 1;
        }
        CC_INSPECT(inFrame);

        auto media1 = bus->connect();
        auto media2 = bus->connect();

        media1->writeFrame(inFrame);
        CC_VERIFY(media2->waitFrame());

        auto outFrame = CanFrame::create();
        media2->readFrame(outFrame);

        CC_INSPECT(outFrame);
        CC_VERIFY(inFrame->equals(outFrame));

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
            auto frame = CanFrame::create();
            int m = 0;
            while (monitorMedia->readFrame(frame)) {
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
            auto frame = CanFrame::create();
            while (echoMedia->readFrame(frame)) {
                frame->setCanId(0xCBA);
                echoMedia->writeFrame(frame);
            }
        });

        // generate n messages
        auto generatorMedia = bus->connect();
        auto generator = Worker::start([=]{
            auto frame = CanFrame::create();
            frame->setCanId(0xABC);
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
