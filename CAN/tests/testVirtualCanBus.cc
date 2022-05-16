#include <cc/VirtualCanBus>
#include <cc/Thread>
#include <cc/Semaphore>
#include <cc/testing>
#include <cc/DEBUG>

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

            auto media1 = bus.connect();
            auto media2 = bus.connect();

            media1.writeFrame(inFrame);
            CC_VERIFY(media2.waitFrame());

            CanFrame outFrame;
            media2.readFrame(&outFrame);

            CC_INSPECT(outFrame);
            CC_VERIFY(inFrame == outFrame);

            bus.shutdown();
        }
    };

    return TestSuite{argc, argv}.run();
}
