#include <cc/debug>
#include <cc/stdio>
#include <cc/map>

namespace cc { template class map<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    map<int> a;
    for (int i = 0; i < 10; ++i)
        a->insert(i, i * i);

    CC_INSPECT(a);

    map<int> b = { KeyValue<int>{1, 1}, KeyValue<int>{2, 2}, KeyValue<int>{3, 4}, KeyValue<int>{4, 6} };
    map<int> c = b;

    for (auto it = b->begin(); it; ++it) ++it->value();
    // for (auto &x: b) ++x->value();
    CC_INSPECT(b);
    CC_INSPECT(c);

    CC_INSPECT(b == c);
    CC_INSPECT(a == b);
    CC_INSPECT(a < b);
    CC_INSPECT(b < a);

    return 0;
}
