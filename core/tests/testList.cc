#include <cc/List>
#include <cc/testing>

namespace cc { template class List<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "Insertion",
        []{
            const int n = 20;

            List<int> list;
            for (int i = 0; i < n; ++i) list << i;

            CC_CHECK(list.count() == n);

            for (int i = 0; i < n; ++i)
                CC_CHECK_EQUALS(list.at(i), i);
        }
    };

    TestCase {
        "SortingAscending",
        []{
            List<int> a;
            Random random{0};
            for (int i = 0; i < 20; ++i)
                a.append(random.get(0, 99));

            auto b = a.sorted();
            CC_INSPECT(a);
            CC_INSPECT(b);

            CC_CHECK(a.count() == b.count());

            for (int i = 0; i < b.count() - 1; ++i) {
                CC_VERIFY(b.at(i) <= b.at(i + 1));
                CC_VERIFY(a.find(b.at(i)));
            }
        }
    };

    TestCase {
        "CopyOnWrite",
        []{
            List<int> a;
            Random random{0};
            for (int i = 0; i < 20; ++i)
                a.append(random.get(0, 99));
            // List<int> b { a };
            List<int> b = a;
            b.append(4);
            CC_INSPECT(a);
            CC_INSPECT(b);
            CC_CHECK(a.count() + 1 == b.count());
            for (auto pos = a.head(); pos; ++pos)
                CC_VERIFY(a.at(pos) == b.at(+pos));

            CC_CHECK(sizeof(List<int>) == sizeof(void *));
        }
    };

    return TestSuite{argc, argv}.run();
}
