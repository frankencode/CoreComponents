#include <cc/VirtualCanBus>
#include <cc/Thread>
#include <cc/Semaphore>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "SimpleReadWriteTest",
        []{
            CC_INSPECT(sizeof(CanFrame));

            VirtualCanBus bus;

            CanFrame inFrame{0x123};
            {
                for (int i = 0; i < inFrame.size(); ++i) {
                    inFrame[i] = i + 1;
                }
            }
            CC_INSPECT(inFrame);

            auto bus1 = bus.connect();
            auto bus2 = bus.connect();

            bus1.write(inFrame);
            CC_VERIFY(bus2.wait());

            CanFrame outFrame;
            bus2.read(&outFrame);

            CC_INSPECT(outFrame);
            CC_VERIFY(inFrame == outFrame);

            bus.shutdown();
        }
    };

    return TestSuite{argc, argv}.run();
}
