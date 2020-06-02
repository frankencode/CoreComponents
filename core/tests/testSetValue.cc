#include <cc/debug>
#include <cc/stdio>
#include <cc/SetValue>

namespace cc { template class SetValue<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    SetValue<int> a;
    for (int i = 0; i < 10; ++i)
        a << i;

    CC_INSPECT(a);

    SetValue<int> b = a;
    int x, y;
    a >> x >> y;

    CC_INSPECT(a);
    CC_INSPECT(b);

    CC_INSPECT(a == b);
    CC_INSPECT(a < b);
    CC_INSPECT(b < a);

    SetValue<String> c = { "A", "B", "C" };

    CC_INSPECT(c);

    return 0;
}
