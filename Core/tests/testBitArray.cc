#include <cc/BitArray>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "BitSetAndGet",
        []{
            CC_CHECK(BitArray{}.count() == 0);

            BitArray bits { 23 };
            for (int i = 0; i < bits.count(); ++i) {
                bits[i] = i & 1;
            }
            for (int i = 0; i < bits.count(); ++i) {
                CC_VERIFY(bits[i] == (i & 1));
            }

            for (auto x: bits) x.toggle();

            BitArray other = bits.copy();
            other.fill(false);

            for (int i = 0; i < bits.count(); ++i) {
                CC_VERIFY(bits[i] != (i & 1));
            }
        }
    };

    return TestSuite{argc, argv}.run();
}
