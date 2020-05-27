#include <cc/debug>
#include <cc/stdio>
#include <cc/map>

namespace cc { template class map<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    map<int> a;
    for (int i = 0; i < 10; ++i)
        a << pair<int>{i, i * i};

    CC_INSPECT(a);

    map<int> b = { pair<int>{1, 1}, pair<int>{2, 2}, pair<int>{3, 4}, pair<int>{4, 6} };
    map<int> c = b;

    for (auto i = b->begin(); i; ++i) ++i->value();
    CC_INSPECT(b);
    CC_INSPECT(c);

    CC_INSPECT(b == c);
    CC_INSPECT(a == b);
    CC_INSPECT(a < b);
    CC_INSPECT(b < a);

    return 0;
}
