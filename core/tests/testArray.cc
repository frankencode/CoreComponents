#include <cc/Array>
#include <cc/testing>

namespace cc { template class Array<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "SelectionIteration",
        []{
            Array<int> a = Array<int>::allocate(10);
            for (int i = 0; i < a.count(); ++i) a[i] = i;

            for (int i = 0; i < a.count(); ++i)
                CC_CHECK_EQUALS(a[i], i);

            auto b = a.select(1, 3);
            for (int i = 0; i < b.count(); ++i)
                CC_CHECK_EQUALS(b[i], b.offset() + i);

            CC_VERIFY(sizeof(Array<int>) == sizeof(void *));
        }
    };

    return TestSuite{argc, argv}.run();
}
