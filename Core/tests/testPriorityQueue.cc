#include <cc/PriorityQueue>

namespace cc { template class PriorityQueue<const char *>; }

int main(int argc, char *argv[])
{
    using namespace cc;
    PriorityQueue<int> queue;

    return 0;
}
