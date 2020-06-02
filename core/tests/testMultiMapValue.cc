#include <cc/debug>
#include <cc/stdio>
#include <cc/MultiMapValue>

namespace cc { template class MultiMapValue<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    MultiMapValue<int> a;
    for (int i = 0; i < 10; ++i)
        a->insert(i, i * i);

    CC_INSPECT(a);

    MultiMapValue<int> b = { KeyValue<int>{1, 1}, KeyValue<int>{1, 1}, KeyValue<int>{2, 2}, KeyValue<int>{3, 4}, KeyValue<int>{4, 6} };
    MultiMapValue<int> c = b;

    for (auto i = b->begin(); i; ++i) ++i->value();
    CC_INSPECT(b);
    CC_INSPECT(c);

    CC_INSPECT(b == c);
    CC_INSPECT(a == b);
    CC_INSPECT(a < b);
    CC_INSPECT(b < a);

    return 0;
}
