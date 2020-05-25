#include <cc/debug>
#include <cc/stdio>
#include <cc/queue>

namespace cc { template class queue<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    queue<int> a;

    for (int i = 0; i < 10; ++i)
        a->pushBack(i);

    while (a->count() > 0)
        fout() << a->popFront() << nl;

    return 0;
}
