#include <cc/testing/TestSuite>
#include <cc/BucketList>
#include <cc/bucket_internals>
#include <cc/debug>
#include <cc/File>
#include <time.h>
#include <list>

using namespace cc;
using namespace cc::testing;

void print(const BucketList<int> *list)
{
    Format f;
    for (int i = 0; i < list->count(); ++i)
        f << list->at(i);
    ferr() << "(" << f->join(", ") << ")" << nl;
}

class PushBackPopBackTest: public TestCase
{
    void run() override
    {
        const int n = 1024;
        Local<BucketList<int>> list;
        for (int i = 0; i < n; ++i) {
            list->push(list->count(), i);
            for (int k = 0; k <= i; ++k)
                CC_VERIFY(list->at(k) == k);
        }
        CC_VERIFY(list->count() == n);
        CC_INSPECT(list->count());
        File::open("tree_push_back.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(&list));
        #if 0
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n - i; ++k)
                CC_VERIFY(list->at(k) == k);
            list->pop(list->count() - 1);
        }
        CC_VERIFY(list->count() == 0);
        #endif
    }
};

int main(int argc, char **argv)
{
    #if 0 // def NDEBUG
    {
        const int h = 100000;
        const int n = 256;

        #if 1
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

        #if 1
        for (int k = 0; k < h; ++k)
        {
            Local<BucketList<int>> list;
            for (int i = 0; i < n; ++i)
                list->push(i, i);
        }
        {
            Local<BucketList<int>> list;
            auto tx = ::clock();
            for (int i = 0; i < n; ++i)
                list->push(i, i);
            tx = ::clock() - tx;
            CC_INSPECT(tx);
        }
        #endif
    }
    #endif
    #if 0 // def NDEBUG
    {
        const int n = 256;
        const int w = 1000;
        {
            const int m = 1000000;
            uint32_t x[m];
            for (int i = 1; i < m; ++i) x[i] = (x[i - 1] + 1) % 50;
            CC_INSPECT(x[m - 1]);
        }
        {
            Local<BucketList<int>> list;
            for (int i = 0; i < n; ++i) list->push(i, i);
            auto tx = ::clock();
            int h = 0;
            for (int i = 0; i < w; ++i) {
                for (int x: list) h += x;
            }
            tx = ::clock() - tx;
            CC_INSPECT(h);
            CC_INSPECT(tx);
        }
        {
            std::list<int> list;
            for (int i = 0; i < n; ++i) list.push_back(i);
            auto ts = ::clock();
            int h = 0;
            for (int i = 0; i < w; ++i) {
                for (int x: list) h += x;
            }
            ts = ::clock() - ts;
            CC_INSPECT(h);
            CC_INSPECT(ts);
        }
    }
    return 0;
    #endif
    #if 0
    Local<BucketList<int>> list;
    const int n = 33;
    for (int i = 0; i < n; ++i) {
        list->push(i, i);
        print(list);
    }
    CC_INSPECT(list->count());
    list->pop(n - 1);
    File::open("bucket_list.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(&list));
    #endif
    #if 0
    for (int i = n - 1; i >= 0; --i) {
        list->pop(i);
        if (i == n - 1) File::open("bucket_list.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(&list));
        CC_INSPECT(list->count());
        // print(list);
    }
    return 0;
    #endif
    #if 0
    CC_TESTSUITE_ADD(PushBackPopBackTest);

    return TestSuite::instance()->run(argc, argv);
    #endif
}
