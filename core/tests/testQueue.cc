#include <cc/Queue>
#include <cc/testing>

namespace cc { template class Queue<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "DequePushBackAndIterate",
        []{
            Queue<int> queue { 0, 1, 2, 3 };
            for (int x = 4; x <= 32; ++x) queue << x;

            for (auto pos = queue.head(); pos; ++pos)
                CC_CHECK_EQUALS(queue.at(pos), +pos);
        }
    };

    return TestSuite{argc, argv}.run();
}
