#include <cc/can_open/Crc16Sink>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;
    using namespace cc::can_open;

    TestCase {
        "SimpleSum",
        []{
            CC_VERIFY(crc16("123456789") == 0x31C3U);
        }
    };

    return TestSuite{argc, argv}.run();
}
