#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/Random>
#include <cc/SetInstance>
#include <cc/bucket_internals>
#include <cc/File>
#include <cc/ArrayInstance>
#include <time.h>
#include <set>
#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

using namespace cc;
using namespace cc::testing;

int sum(const ArrayInstance<int> *items)
{
    int sum = 0;
    for (int x: items) sum += x;
    return sum;
}

int sum(const SetInstance<int> *items)
{
    int sum = 0;
    for (int x: items) sum += x;
    return sum;
}

class InsertionRemovalTest: public TestCase
{
protected:
    virtual void scramble(ArrayInstance<int> *test) = 0;
    virtual void dotify(const SetInstance<int> *set) {}

private:
    void run() override
    {
        const int n = 513;

        Local<ArrayInstance<int>> test{n};
        for (int i = 0; i < n; ++i)
            test->at(i) = i;

        int checkSum = sum(test);

        scramble(&test);

        Local<SetInstance<int>> set;
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
    void scramble(ArrayInstance<int> *) override
    {}

    void dotify(const SetInstance<int> *set)
    {
        File{"set_asc_ins.dot", FileOpen::Overwrite}->write(bucket::Internals::dotify(set));
    }
};

class DescendingInsertionRemovalTest: public InsertionRemovalTest
{
    void scramble(ArrayInstance<int> *test) override
    {
        test->reverseInsitu();
    }
};

class RandomInsertionRemovalTest: public InsertionRemovalTest
{
    void scramble(ArrayInstance<int> *test) override
    {
        Random{0}->scramble(test);
    }

    void dotify(const SetInstance<int> *set)
    {
        File{"set_rnd_ins.dot", FileOpen::Overwrite}->write(bucket::Internals::dotify(set));
    }
};

int main(int argc, char **argv)
{
    #if 0 // def NDEBUG
    const int n = 129;
    // const int m = 1000;

    Local<ArrayInstance<int>> test{n};
    for (int i = 0; i < n; ++i)
        test->at(i) = i;

    Random{0}->scramble(&test);
    // test->reverseInsitu();

    CC_INSPECT(sum(test));

    Local<SetInstance<int>> set;
    for (auto x: test) set->insert(x);

    File::open("set_rnd_ins.dot", FileOpen::WriteOver)->write(bucket::Internals::dotify(&set));

    CC_INSPECT(sum(set));

    auto tree = bucket::Internals::tree(&set);
    CC_INSPECT(tree->idealLeafCount());

    for (int i = 0; i < tree->idealLeafCount(); ++i) {
        const bucket::Leaf<int> *leaf = static_cast<const bucket::Leaf<int> *>(tree->idealLeafAt(i));
        int j = bucket::Tree::nodeOffset(leaf);
        fout() << i << ", " << j << ": " << leaf->at(0) << "..." << leaf->at(leaf->fill() - 1) << " (#" << leaf->fill() << ")" << nl;
    }

    bucket::SearchShim<int> shim{tree};
    for (int i = 0; i < n; ++i) {
        bucket::Weight j = 0;
        bool found = binarySearch(&shim, i, &j);
        fout() << i << " => " << j << " (" << found << ")" << nl;
    }

    /*for (int i = 0; i < tree->idealCount(); ++i)
    {
        int j = tree->idealToReal(i);
        fout() << i << ", " << j << ": " << tree->idealAt<int>(i) << nl;
    }*/

    return 0;
    #endif

    #ifdef NDEBUG
    #if 1
    CC_INSPECT(sizeof(SetInstance<int>));
    CC_INSPECT(sizeof(bucket::Node));
    CC_INSPECT(sizeof(bucket::Branch));
    CC_INSPECT(sizeof(bucket::Leaf<int>));
    CC_INSPECT(sizeof(SetInstance<int>)/16.);
    CC_INSPECT(sizeof(bucket::Node)/16.);
    CC_INSPECT(sizeof(bucket::Branch)/16.);
    CC_INSPECT(sizeof(bucket::Leaf<int>)/16.);
    #endif

    const int n = 1000;  // number of items
    const int m = 1000;  // repetition

    typedef int Item;
    Local<ArrayInstance<Item>> test{n};
    for (int i = 0; i < n; ++i)
        test->at(i) = i;

    //auto test_2 = test->select(n/2, n);
    //Random{0}->scramble<ArrayInstance<int>>(test_2);

    Random{0}->scramble(&test);

    // test->reverseInsitu();

    // CC_INSPECT(sum(test));

    #if 0
    {
        auto tl = ::clock();
        for (int j = 0; j < m; ++j) {
            Local<SetInstance<int>> set;
            for (auto x: test) set->insert(x);
        }
        tl = ::clock() - tl;
        CC_INSPECT(tl);
    }
    #endif
    uint64_t txm = 0; // std::numeric_limits<uint64_t>::max();
    uint64_t tsm = 0; // std::numeric_limits<uint64_t>::max();
    int64_t dummy = 0;

    for (int i = 0; i < m; ++i) {
        {
            uint64_t ts = __rdtsc();
            //{
                std::set<Item> set;
                // std::multiset<int> set;
                for (auto x: test) set.insert(x);
            //}
            ts = __rdtsc() - ts;
            dummy += set.size();
            // CC_INSPECT(ts);

            tsm += ts;
        }
        {
            uint64_t tx = __rdtsc();
            //{
                Local<SetInstance<Item>> set;
                for (auto x: test) set->insert(x);
            //}
            tx = __rdtsc() - tx;
            dummy += set->count();
            // CC_INSPECT(tx);

            txm += tx;
        }
    }

    CC_INSPECT(dummy);

    tsm /= m;
    txm /= m;

    CC_INSPECT(double(tsm)/double(txm));
    CC_INSPECT(double(txm)/double(tsm));

    return 0;
    #endif

    #ifndef NDEBUG
    CC_TESTSUITE_ADD(AscendingInsertionRemovalTest);
    CC_TESTSUITE_ADD(DescendingInsertionRemovalTest);
    CC_TESTSUITE_ADD(RandomInsertionRemovalTest);

    return TestSuite::instance()->run(argc, argv);
    #endif
}
