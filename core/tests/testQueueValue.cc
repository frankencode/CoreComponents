#include <cc/debug>
#include <cc/stdio>
#include <cc/Queue>

namespace cc { template class Queue<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    Queue<int> a;

    for (int i = 0; i < 10; ++i) {
        a->pushBack(i);
        CC_INSPECT(a);
    }

    while (a->count() > 0) {
        a->popFront();
        CC_INSPECT(a);
    }

    return 0;
}
