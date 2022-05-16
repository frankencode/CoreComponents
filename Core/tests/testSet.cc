#include <cc/Set>
#include <cc/testing>

namespace cc { template class Set<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "InsertionSimple",
        []{
            Set<int> a;
            a.insert(1);
            a.insert(2);
            a.insert(2);
            a.insert(3);

            CC_CHECK(a.count() == 3);
            CC_CHECK(a.at(0) == 1);
            CC_CHECK(a.at(1) == 2);
            CC_CHECK(a.at(2) == 3);
        }
    };

    return TestSuite{argc, argv}.run();
}
