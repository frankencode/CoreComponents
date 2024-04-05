#include <cc/can_open/sdo/protocol>
#include <cc/can_open/EchoNode>
#include <cc/can_open/Annotator>
#include <cc/VirtualCanBus>
#include <cc/CanLogger>
#include <cc/testing>

int main(int argc, char* argv[])
{
    using namespace cc;

    auto readWriteTest = [](can_open::Selector selector, const Bytes &inData)
    {
        Bytes outData;
        {
            const int serverId = 1;

            VirtualCanBus bus;

            CanLogger logger { bus.connect() };
            logger.addAnnotator(can_open::Annotator{});
            logger.start();

            can_open::EchoNode node { bus.connect(), serverId };
            node.start();

            CanMedia media = bus.connect();

            can_open::sdo::write(media, serverId, selector, inData);

            outData = can_open::sdo::read(media, serverId, selector);

            bus.shutdown();
        }
        CC_VERIFY(inData == outData);
    };

    TestCase {
        "test.can_open.sdo.expedited",
        [=]{
            readWriteTest(1, String{"1234"});
        }
    };

    TestCase {
        "test.can_open.sdo.segmented",
        [=]{
            readWriteTest(1, String{"Hello, world!"});
        }
    };

    return TestSuite{argc, argv}.run();
}
