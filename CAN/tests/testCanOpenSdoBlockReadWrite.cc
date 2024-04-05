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

            can_open::sdo::blockWrite(media, serverId, selector, inData);

            outData = can_open::sdo::blockRead(media, serverId, selector);

            bus.shutdown();
        }
        CC_VERIFY(inData == outData);
    };

    TestCase {
        "test.can_open.sdo.block_wise_write.expedited_read_fallback",
        [=]{
            readWriteTest(1, String{"1234"});
        }
    };

    TestCase {
        "test.can_open.sdo.block_wise_write.segmented_read_fallback",
        [=]{
            readWriteTest(1, String{"Hello, world!"});
        }
    };

    TestCase {
        "test.can_open.sdo.block_wise_read_write",
        [=]{
            Bytes data { Dim<>{0x700} };
            {
                Random random;
                for (int i = 0; i < data.count(); ++i) {
                    data[i] = i /*random(0, 0xFF)*/;
                }
            }
            readWriteTest(1, data);
        }
    };

    return TestSuite{argc, argv}.run();
}
