#include <cc/AppendList>
#include <cc/testing>

namespace cc { template class AppendList<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "InsertionSimple",
        []{
            AppendList<int> a;
            for (int i = 0; i < 8; ++i) a.pushBack(i);
            for (auto x: a) CC_INSPECT(x);
        }
    };

    return TestSuite{argc, argv}.run();
}
