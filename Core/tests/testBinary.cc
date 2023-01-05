#include <cc/binary>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "BitFieldValues",
        []{
            CC_CHECK((bit<3, 0,uint8_t>(0x10)) == 0);
            CC_CHECK(bitmask<uint8_t>(4) == 0xF);
        }
    };

    return TestSuite{argc, argv}.run();
}
