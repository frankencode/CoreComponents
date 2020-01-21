#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/Random>
#include <cc/BucketSet>
#include <cc/bucket_internals>
#include <cc/File>
#include <cc/Array>
#include <cc/Set>
#include <time.h>
#include <set>
#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

using namespace cc;
using namespace cc::testing;

void print(const BucketSet<int> *set)
{
    Format f;
    for (int x: set) f << x;
    ferr() << "(" << f->join(", ") << ")" << nl;
}

int sum(const Array<int> *items)
{
    int sum = 0;
    for (int x: items) sum += x;
    return sum;
}

int sum(const BucketSet<int> *items)
{
    int sum = 0;
    for (int x: items) sum += x;
    return sum;
}

class InsertionRemovalTest: public TestCase
{
protected:
    virtual void scramble(Array<int> *test) = 0;
    virtual void dotify(const BucketSet<int> *set) {}

private:
    void run() override
    {
        const int n = 513;

        Local<Array<int>> test{n};
        for (int i = 0; i < n; ++i)
            test->at(i) = i;

        int checkSum = sum(test);

        scramble(&test);

        Local<BucketSet<int>> set;
        auto t = ::clock();
        for (auto x: test) set->insert(x);
        t = ::clock() - t;
        CC_INSPECT(t);
        dotify(set);

        CC_VERIFY(set->count() == test->count());

        CC_VERIFY(sum(set) == checkSum);

        for (auto x: test) {
            CC_VERIFY(set->contains(x));
            set->remove(x);
            CC_VERIFY(!set->contains(x));
        }

        CC_VERIFY(set->count() == 0);
    }
};

class AscendingInsertionRemovalTest: public InsertionRemovalTest
{
    void scramble(Array<int> *) override
    {}

    void dotify(const BucketSet<int> *set)
    {
        File::open("set_asc_ins.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(set));
    }
};

class DescendingInsertionRemovalTest: public InsertionRemovalTest
{
    void scramble(Array<int> *test) override
    {
        test->reverseInsitu();
    }
};

class RandomInsertionRemovalTest: public InsertionRemovalTest
{
    void scramble(Array<int> *test) override
    {
        Local<Random>{0}->scramble(test);
    }

    void dotify(const BucketSet<int> *set)
    {
        File::open("set_rnd_ins.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(set));
    }
};

int main(int argc, char **argv)
{
    #if 0 // def NDEBUG
    // CPU warmup
    /*{
        auto tw = ::clock();
        const int m = 100000000;
        volatile uint32_t *x = new uint32_t[m];
        for (int i = 1; i < m; ++i) x[i] = (x[i - 1] + 1) % 50;
        tw = ::clock() - tw;
        CC_INSPECT(tw);
        CC_INSPECT(x[m - 1]);
        delete[] x;
    }*/

    const int n = 1000;
    // const int m = 1000;

    Local<Array<int>> test{n};
    for (int i = 0; i < n; ++i)
        test->at(i) = i;

    Local<Random>{0}->scramble(&test);
    // test->reverseInsitu();

    CC_INSPECT(sum(test));

    #if 0
    {
        Local<BucketSet<int>> set;
        for (auto x: test) set->insert(x);

        File::open("set_rnd_ins.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(&set));

        CC_INSPECT(sum(set));

        auto tree = bucket::Internals::tree(&set);
        CC_INSPECT(tree->idealCount());

        for (int i = 0; i < tree->idealCount(); ++i)
        {
            int j = tree->idealToReal(i);
            fout() << i << ", " << j << ": " << tree->idealAt<int>(i) << nl;
        }
    }

    return 0;
    #endif

    #if 0
    {
        auto tl = ::clock();
        for (int j = 0; j < m; ++j) {
            Local<Set<int>> set;
            for (auto x: test) set->insert(x);
        }
        tl = ::clock() - tl;
        CC_INSPECT(tl);
    }
    #endif
    uint64_t tx1, ts1, tx2, ts2;

    {
        tx1 = __rdtsc();
        //{
            Local<BucketSet<int>> set;
            for (auto x: test) set->insert(x);
        //}
        tx1 = __rdtsc() - tx1;
        CC_INSPECT(set->count());
        CC_INSPECT(tx1);

        #if 0
        for (int i = n - 1; i >= 0; --i) {
            set->remove(test[i]);
            // print(set);
        }
        #endif
    }

    {
        ts1 = __rdtsc();
        //{
            std::set<int> set;
            for (auto x: test) set.insert(x);
        //}
        ts1 = __rdtsc() - ts1;
        CC_INSPECT(set.size());
        CC_INSPECT(ts1);
    }

   {
        tx2 = __rdtsc();
        //{
            Local<BucketSet<int>> set;
            for (auto x: test) set->insert(x);
        //}
        tx2 = __rdtsc() - tx2;
        CC_INSPECT(set->count());
        CC_INSPECT(tx2);

        #if 0
        for (int i = n - 1; i >= 0; --i) {
            set->remove(test[i]);
            // print(set);
        }
        #endif
    }

    {
        ts2 = __rdtsc();
        //{
            std::set<int> set;
            for (auto x: test) set.insert(x);
        //}
        ts2 = __rdtsc() - ts2;
        CC_INSPECT(set.size());
        CC_INSPECT(ts2);
    }

    /*auto ts = ts1 + ts2;
    auto tx = tx1 + tx2;*/
    auto ts = ts1 < ts2 ? ts1 : ts2;
    auto tx = tx1 < tx2 ? tx1 : tx2;

    CC_INSPECT(double(ts)/double(tx));
    CC_INSPECT(double(tx)/double(ts));

    return 0;
    #endif

    #if 1
    CC_TESTSUITE_ADD(AscendingInsertionRemovalTest);
    CC_TESTSUITE_ADD(DescendingInsertionRemovalTest);
    CC_TESTSUITE_ADD(RandomInsertionRemovalTest);

    return TestSuite::instance()->run(argc, argv);
    #endif
}
