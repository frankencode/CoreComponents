#include <cc/testing/TestSuite>
#include <cc/ListInstance>
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

void print(const ListInstance<int> *list)
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
        Local<ListInstance<int>> list;
        for (int i = 0; i < n; ++i) {
            list->insertAt(list->count(), i);
            for (int k = 0; k <= i; ++k)
                CC_VERIFY(list->at(k) == k);
        }
        CC_VERIFY(list->count() == n);
        File{"tree_push_back.dot", FileOpen::Overwrite}->write(bucket::Internals::dotify(&list));
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n - i; ++k)
                CC_VERIFY(list->at(k) == k);
            list->removeAt(list->count() - 1);
        }
        CC_VERIFY(list->count() == 0);
    }
};

class PushFrontPopFrontTest: public TestCase
{
    void run() override
    {
        const int n = 1024;
        Local<ListInstance<int>> list;
        for (int i = 0; i < n; ++i) {
            list->insertAt(0, i);
            for (int k = 0; k <= i; ++k)
                CC_VERIFY(list->at(k) == i - k);
        }
        CC_VERIFY(list->count() == n);
        File{"tree_push_front.dot", FileOpen::Overwrite}->write(bucket::Internals::dotify(&list));
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n - i; ++k)
                CC_VERIFY(list->at(k) == n - i - 1 - k);
            list->removeAt(0);
        }
        CC_VERIFY(list->count() == 0);
    }
};

class PushFrontPopBackTest: public TestCase
{
    void run() override
    {
        const int n = 1024;
        Local<ListInstance<int>> list;
        for (int i = 0; i < n; ++i) {
            list->insertAt(0, i);
            for (int k = 0; k <= i; ++k)
                CC_VERIFY(list->at(k) == i - k);
        }
        CC_VERIFY(list->count() == n);
        // File::open("tree_push_front.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(&list));
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n - i; ++k)
                CC_VERIFY(list->at(k) == n - 1 - k);
            list->removeAt(list->count() - 1);
        }
        CC_VERIFY(list->count() == 0);
    }
};

class PushBackPopFrontTest: public TestCase
{
    void run() override
    {
        const int n = 1024;
        Local<ListInstance<int>> list;
        auto t = ::clock();
        for (int i = 0; i < n; ++i) {
            list->insertAt(list->count(), i);
        }
        t = ::clock() - t;
        CC_INSPECT(t);
        CC_VERIFY(list->count() == n);
        // File::open("tree_push_back.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(&list));
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n - i; ++k)
                CC_VERIFY(list->at(k) == k + i);
            list->removeAt(0);
        }
        CC_VERIFY(list->count() == 0);
    }
};

class PushPopRandomTest: public TestCase
{
    void run() override
    {
        const int n = 1024;
        Local<ListInstance<int>> list;
        Random random{0};
        auto t = ::clock();
        for (int i = 0; i < n; ++i) {
            auto r = random->get(0, i + 1);
            list->insertAt(r, i);
        }
        t = ::clock() - t;
        CC_INSPECT(t);
        CC_VERIFY(list->count() == n);
        File{"tree_random.dot", FileOpen::Overwrite}->write(bucket::Internals::dotify(&list));
        for (int i = 0; i < n; ++i) {
            auto r = random->get(0, list->count() - 1);
            list->removeAt(r);
        }
        CC_VERIFY(list->count() == 0);
    }
};

int main(int argc, char **argv)
{
    #if 0
    auto primes = ListInstance<unsigned>::create();
    const unsigned b = std::numeric_limits<unsigned>::max();
    unsigned x = 2, y = x;
    while (x <= b) {
        bool isPrime = true;
        for (auto p: primes) {
            if (x % p == 0) {
                isPrime = false;
                break;
            }
        }
        if (isPrime) {
            primes->pushBack(x);
            fout() << primes->count() << ": " << x << ", " << x - y << nl;
            y = x;
        }
        ++x;
    }
    return 0;
    #endif

    #if 1 // def NDEBUG
    {
        const int n = 1024;
        const int h = 1000;

        uint64_t tx, ts;

        for (int k = 0; k < h; ++k)
        {
            Local<ListInstance<int>> list;
            for (int i = 0; i < n; ++i)
                list->pushBack(i);
        }
        {
            Local<ListInstance<int>> list;
            tx = ::clock();
            for (int i = 0; i < n; ++i)
                list->pushBack(i);
            tx = ::clock() - tx;
            // CC_INSPECT(tx);
        }

        for (int k = 0; k < h; ++k)
        {
            std::list<int> list;
            for (int i = 0; i < n; ++i)
                list.push_back(i);
        }
        {
            std::list<int> list;
            ts = ::clock();
            for (int i = 0; i < n; ++i)
                list.push_back(i);
            ts = ::clock() - ts;
            // CC_INSPECT(ts);
        }

        // CC_INSPECT(double(tx)/ts);
        CC_INSPECT(double(ts)/tx);
    }
    return 0;
    #endif
    #if 0 // def NDEBUG
    {
        const int n = 100000;
        const int w = 1000;
        uint64_t tx, ts;
        {
            const int m = 1000000;
            uint32_t x[m];
            for (int i = 1; i < m; ++i) x[i] = (x[i - 1] + 1) % 50;
            CC_INSPECT(x[m - 1]);
        }
        {
            Local<ListInstance<int>> list;
            for (int i = 0; i < n; ++i) list->pushBack(i);
            tx = ::clock();
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
            ts = ::clock();
            int h = 0;
            for (int i = 0; i < w; ++i) {
                for (int x: list) h += x;
            }
            ts = ::clock() - ts;
            CC_INSPECT(h);
            CC_INSPECT(ts);
        }

        CC_INSPECT(double(ts)/tx);
        CC_INSPECT(double(tx)/ts);
    }
    return 0;
    #endif
    #if 0
    Local<ListInstance<int>> list;
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
    #ifndef NDEBUG
    CC_TESTSUITE_ADD(PushBackPopBackTest);
    CC_TESTSUITE_ADD(PushFrontPopFrontTest);
    CC_TESTSUITE_ADD(PushFrontPopBackTest);
    CC_TESTSUITE_ADD(PushBackPopFrontTest);
    CC_TESTSUITE_ADD(PushPopRandomTest);

    return TestSuite::instance()->run(argc, argv);
    #endif
}
