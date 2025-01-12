#include <cc/MultiSet>
#include <cc/testing>

namespace cc { template class MultiSet<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "InsertionSimple",
        []{
            MultiSet<int> a;
            a.insert(3);
            a.insert(2);
            a.insert(2);
            a.insert(1);

            CC_CHECK(a.count() == 4);
            CC_CHECK(a.at(0) == 1);
            CC_CHECK(a.at(1) == 2);
            CC_CHECK(a.at(2) == 2);
            CC_CHECK(a.at(3) == 3);

            List<int> b { std::move(a) };
            CC_CHECK(b.count() == 4);
            CC_CHECK(b.at(0) == 1);
            CC_CHECK(b.at(1) == 2);
            CC_CHECK(b.at(2) == 2);
            CC_CHECK(b.at(3) == 3);
        }
    };

    return TestSuite{argc, argv}.run();
}
