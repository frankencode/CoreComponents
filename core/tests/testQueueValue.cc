#include <cc/debug>
#include <cc/stdio>
#include <cc/QueueValue>

namespace cc { template class QueueValue<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    QueueValue<int> a;

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
