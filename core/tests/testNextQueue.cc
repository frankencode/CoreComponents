#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/NextQueue>
#include <list>
#include <deque>
#include <queue>

using namespace cc;
using namespace cc::testing;

int main(int argc, char **argv)
{
    // TODO: make up some automated test case
    #if 0
    Local<NextQueue<int>> queue;
    const int n = 10000;
    for (int i = 0; i < n; ++i) {
        fout() << i << nl;
        queue->pushBack(i);
    }
    for (int i = 0; i < n; ++i) {
        int x = queue->popFront();
        assert(x == i);
        fout() << x << nl;
    }
    #endif

    // CC_INSPECT(sizeof(NextQueue<int>::Node));

    const int h = 1000;
    const int n = 100000;

    #if 0
    for (int k = 0; k < h; ++k)
    {
        std::list<int> list;
        for (int i = 0; i < n; ++i)
            list.push_back(i);
    }
    {
        std::list<int> list;
        auto ts = ::clock();
        for (int i = 0; i < n; ++i)
            list.push_back(i);
        ts = ::clock() - ts;
        CC_INSPECT(ts);
    }
    #endif
    #if 0
    for (int k = 0; k < h; ++k)
    {
        std::deque<int> list;
        for (int i = 0; i < n; ++i)
            list.push_back(i);
    }
    {
        std::deque<int> list;
        auto ts = ::clock();
        for (int i = 0; i < n; ++i)
            list.push_back(i);
        ts = ::clock() - ts;
        CC_INSPECT(ts);
    }
    #endif
    #if 0
    for (int k = 0; k < h; ++k)
    {
        Local<NextQueue<int>> queue;
        for (int i = 0; i < n; ++i)
            queue->pushBack(i);
    }
    {
        Local<NextQueue<int>> queue;
        auto tq = ::clock();
        for (int i = 0; i < n; ++i)
            queue->pushBack(i);
        tq = ::clock() - tq;
        CC_INSPECT(tq);
    }
    #endif
    #if 0
    for (int k = 0; k < h; ++k)
    {
        std::queue<int> list;
        for (int i = 0; i < n; ++i)
            list.push(i);
    }
    {
        std::queue<int> list;
        auto ts = ::clock();
        for (int i = 0; i < n; ++i)
            list.push(i);
        ts = ::clock() - ts;
        CC_INSPECT(ts);
    }
    #endif

    return 0;
}
