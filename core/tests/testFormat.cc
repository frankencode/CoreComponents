#include <cc/str>
#include <cc/input>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "FloatingPointFormatting",
        []{
            CC_CHECK(fixed(1.23, 3) == "1.230");
            CC_CHECK(fixed(-10.01, 4) == "-10.0100");
            CC_CHECK(fixed(0.01, 2) == "0.01");
            CC_CHECK(fixed(9.9999, 3, 3) == " 10.000");
            CC_CHECK(fixed(-9.9999, 3, 3) == "-10.000");
        }
    };

    TestCase {
        "IntegerScanningAndFormatting",
        []{
            CC_CHECK(readNumber<int>(       dec(123)) == 123);
            CC_CHECK(readNumber<int>("0x" + hex(123)) == 123);
            CC_CHECK(readNumber<int>( "0" + oct(123)) == 123);
            CC_CHECK(readNumber<int>("0b" + bin(123)) == 123);
        }
    };

    return TestSuite{argc, argv}.run();
};
