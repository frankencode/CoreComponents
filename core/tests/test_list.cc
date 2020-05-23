#include <cc/debug>
#include <cc/stdio>
#include <cc/list>

namespace cc { template class list<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    list<int> a;
    for (int i = 0; i < 10; ++i)
        a << i;

    list<int> b = a;

    a[0] = -1;

    for (int i = 0; i < 10; ++i)
        fout("a[%%] = %%") << i << a[i] << nl;

    fout() << nl;
    fout("b = %%") << b << nl;

    list<int> c = { 1, 2, 4, 6 };
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
