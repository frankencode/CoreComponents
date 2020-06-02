#include <cc/debug>
#include <cc/stdio>
#include <cc/PriorityQueueValue>

namespace cc { template class PriorityQueueValue<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    PriorityQueueValue<int> a;

    for (int i = 0; i < 10; ++i)
        a->pushBack(i, (i % 2));

    while (a->count() > 0)
        fout() << a->popFront() << nl;

    return 0;
}
