#include <cc/debug>
#include <cc/stdio>
#include <cc/List>

namespace cc { template class List<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    List<int> a;
    for (int i = 0; i < 10; ++i)
        a << i;

    List<int> b = a;

    a[0] = -1;

    for (int i = 0; i < 10; ++i)
        fout("a[%%] = %%") << i << a[i] << nl;

    fout() << nl;
    fout("b = %%") << b << nl;

    List<int> c = { 1, 2, 4, 6 };

    a = c;
    for (int &x: c) ++x;

    CC_INSPECT(a);
    CC_INSPECT(c);
    CC_INSPECT(a == c);
    CC_INSPECT(a == b);
    CC_INSPECT(a < b);
    CC_INSPECT(b < a);

    return 0;
}
