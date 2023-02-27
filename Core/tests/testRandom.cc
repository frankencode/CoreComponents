#include <cc/Random>
#include <cc/Array>
#include <cc/testing>

namespace cc { template class Queue<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "Rangqd1Correctness",
        []{
            Random random { 0 };

            Array<uint32_t> expected {
                0x3C6EF35F,
                0x47502932,
                0xD1CCF6E9,
                0xAAF95334,
                0x6252E503,
                0x9F2EC686,
                0x57FE6C2D,
                0xA3D95FA8,
                0x81FDBEE7,
                0x94F0AF1A,
                0xCBF633B1
            };

            for (int i = 0; i < 11; ++i) {
                CC_INSPECT(random() == expected(i));
            }
        }
    };

    return TestSuite{argc, argv}.run();
}
