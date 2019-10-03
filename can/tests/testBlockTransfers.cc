#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/Random>
#include <cc/can/VirtualCanBus>
#include <cc/can/EchoNode>
#include <cc/can/CanClient>
#include <cc/can/CanLogger>

using namespace cc;
using namespace cc::can;
using namespace cc::testing;

class BlockTransferTest: public TestCase
{
protected:
    void readWriteTest(const String &inData, int switchThreshold = 0)
    {
        String outData;
        {
            const int serverId = 1;
            const int clientId = 2;

            auto bus = VirtualCanBus::create();

            auto server = EchoNode::create(bus->connect(), serverId);
            auto client = CanClient::create(bus->connect(), clientId);

            auto logger = CanLogger::create(bus->connect());

            client->blockWrite(serverId, 1, inData);
            outData = client->blockRead(serverId, 1, 0x7F, switchThreshold);

            bus->shutdown();
        }
        CC_VERIFY(inData == outData);
    }
};

class SimpleBlockTransferTest: public BlockTransferTest
{
    void run() override
    {
        readWriteTest("1234");
        readWriteTest("Hello, world!");
    }
};

class FullBlockTransferTest: public BlockTransferTest
{
    void run() override
    {
        auto data = String::allocate(0x700);
        {
            auto random = Random::open();
            for (int i = 0; i < data->count(); ++i)
                mutate(data)->byteAt(i) = random->get(0, 0xFF);
        }
        readWriteTest(data);
    }
};

class FallbackTransferTest: public BlockTransferTest
{
    void run() override
    {
        readWriteTest("1234", 84);
        readWriteTest("Hello, world!", 84);
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimpleBlockTransferTest);
    CC_TESTSUITE_ADD(FullBlockTransferTest);
    CC_TESTSUITE_ADD(FallbackTransferTest);
    return TestSuite::instance()->run(argc, argv);
}
