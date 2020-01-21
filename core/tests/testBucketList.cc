#include <cc/testing/TestSuite>
#include <cc/BucketList>
#include <cc/bucket_internals>
#include <cc/debug>
#include <cc/File>
#include <cc/Random>
#include <time.h>
#include <list>

#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

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
        File::open("tree_push_front.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(&list));
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n - i; ++k)
                CC_VERIFY(list->at(k) == k);
            list->pop(list->count() - 1);
        }
        CC_VERIFY(list->count() == 0);
    }
};

class PushFrontPopFrontTest: public TestCase
{
    void run() override
    {
        const int n = 1024;
        Local<BucketList<int>> list;
        for (int i = 0; i < n; ++i) {
            list->push(0, i);
            for (int k = 0; k <= i; ++k)
                CC_VERIFY(list->at(k) == i - k);
        }
        CC_VERIFY(list->count() == n);
        File::open("tree_push_front.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(&list));
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n - i; ++k)
                CC_VERIFY(list->at(k) == n - i - 1 - k);
            list->pop(0);
        }
        CC_VERIFY(list->count() == 0);
    }
};

class PushFrontPopBackTest: public TestCase
{
    void run() override
    {
        const int n = 1024;
        Local<BucketList<int>> list;
        for (int i = 0; i < n; ++i) {
            list->push(0, i);
            for (int k = 0; k <= i; ++k)
                CC_VERIFY(list->at(k) == i - k);
        }
        CC_VERIFY(list->count() == n);
        // File::open("tree_push_front.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(&list));
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n - i; ++k)
                CC_VERIFY(list->at(k) == n - 1 - k);
            list->pop(list->count() - 1);
        }
        CC_VERIFY(list->count() == 0);
    }
};

class PushBackPopFrontTest: public TestCase
{
    void run() override
    {
        const int n = 1024;
        Local<BucketList<int>> list;
        auto t = ::clock();
        for (int i = 0; i < n; ++i) {
            list->push(list->count(), i);
        }
        t = ::clock() - t;
        CC_INSPECT(t);
        CC_VERIFY(list->count() == n);
        // File::open("tree_push_back.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(&list));
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n - i; ++k)
                CC_VERIFY(list->at(k) == k + i);
            list->pop(0);
        }
        CC_VERIFY(list->count() == 0);
    }
};

class PushPopRandomTest: public TestCase
{
    void run() override
    {
        const int n = 1024;
        Local<BucketList<int>> list;
        Local<Random> random{0};
        auto t = ::clock();
        for (int i = 0; i < n; ++i) {
            auto r = random->get(0, i + 1);
            list->push(r, i);
        }
        t = ::clock() - t;
        CC_INSPECT(t);
        CC_VERIFY(list->count() == n);
        File::open("tree_random.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(&list));
        for (int i = 0; i < n; ++i) {
            auto r = random->get(0, list->count() - 1);
            list->pop(r);
        }
        CC_VERIFY(list->count() == 0);
    }
};

int main(int argc, char **argv)
{
    #if 0 // def NDEBUG
    {
        const int n = 256;
        const int h = 1000;

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

    }
    return 0;
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
        auto t = __rdtsc();
        list->push(i, i);
        t = __rdtsc() - t;
        CC_INSPECT(t);
        print(list);
    }
    CC_INSPECT(list->count());
    for (int i = n - 1; i >= 0; --i) {
        auto t = __rdtsc();
        list->pop(i);
        t = __rdtsc() - t;
        CC_INSPECT(t);
        // CC_INSPECT(list->count());
        print(list);
    }
    return 0;
    #endif
    #if 1
    CC_TESTSUITE_ADD(PushBackPopBackTest);
    CC_TESTSUITE_ADD(PushFrontPopFrontTest);
    CC_TESTSUITE_ADD(PushFrontPopBackTest);
    CC_TESTSUITE_ADD(PushBackPopFrontTest);
    CC_TESTSUITE_ADD(PushPopRandomTest);

    return TestSuite::instance()->run(argc, argv);
    #endif
}
