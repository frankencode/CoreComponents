#include <cc/testing/TestSuite>
#include <cc/debug>
#include <cc/stdio>
#include <cc/can/Crc16Sink>

using namespace cc;
using namespace cc::can;
using namespace cc::testing;

class SimpleCorrectnessTest: public TestCase
{
    void run() override
    {
        CC_VERIFY(crc16("123456789") == 0x31C3U);
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimpleCorrectnessTest);

    return TestSuite::instance()->run(argc, argv);
}
