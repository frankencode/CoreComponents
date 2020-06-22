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

        VirtualCanBus bus;

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

        VirtualCanBus bus;
        {
            // echo all messages
            Worker echo{[&bus]{
                CanMedia media = bus->connect();
                for (CanFrame frame; media->readFrame(&frame);) {
                    frame->setCanId(0xCBA);
                    media->writeFrame(frame);
                }
            }};

            // generate n messages
            Worker generator{[&bus]{
                CanMedia media = bus->connect();
                CanFrame frame{0xABC};
                for (int i = 0; i < n; ++i) {
                    for (int k = 0; k < frame->payloadCount(); ++k)
                        frame->payloadAt(k) = i + k + 1;
                    media->writeFrame(frame);
                }
            }};

            // log messages and verify that all messages are transmitted
            CanMedia media = bus->connect();
            int m = 0;
            for (CanFrame frame; media->readFrame(&frame);) {
                ++m;
                fout("(%%) %%\n") << m << frame;
                if (frame->canId() == 0xCBA && frame->payloadAt(0) == n) break;
            }
            bus->shutdown();
            CC_VERIFY(m > n);
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimpleReadWriteTest);
    CC_TESTSUITE_ADD(ComplexReadWriteTest);

    return TestSuite::instance()->run(argc, argv);
}
