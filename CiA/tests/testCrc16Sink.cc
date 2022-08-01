#include <cc/cia/Crc16Sink>
#include <cc/testing>

int main(int argc, char **argv)
{
    using namespace cc;
    using namespace cc::cia;

    TestCase {
        "SimpleSum",
        []{
            CC_VERIFY(crc16("123456789") == 0x31C3U);
        }
    };

    return TestSuite{argc, argv}.run();
}
