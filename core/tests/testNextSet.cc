#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/Random>
#include <cc/NextSet>
#include <cc/File>
#include <cc/Array>
#include <cc/Set>
#include <time.h>
#include <set>

using namespace cc;
using namespace cc::testing;

void print(const NextSet<int> *set)
{
    Format f;
    for (int x: set) f << x;
    ferr() << "(" << f->join(", ") << ")" << nl;
}

template<class Items>
typename Items::Item sum(const Items *items) {
    typedef typename Items::Item Item;
    Item sum = 0;
    for (const Item &x: items) sum += x;
    return sum;
}

template<class Items>
typename Items::Item sum(Local<Items> &items) { return sum(&items); }

class InsertionRemovalTest: public TestCase
{
protected:
    virtual void scramble(Array<int> *test) = 0;

private:
    void run() override
    {
        const int n = 129;

        Local<Array<int>> test{n};
        for (int i = 0; i < n; ++i)
            test->at(i) = i;

        int checkSum = sum(test);

        scramble(&test);

        Local<NextSet<int>> set;
        auto t = ::clock();
        for (auto x: test) set->insert(x);
        t = ::clock() - t;
        CC_INSPECT(t);

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
};

int main(int argc, char **argv)
{
    #if 1
    // CPU warmup
    {
        const int m = 1000000;
        uint32_t x[m];
        for (int i = 1; i < m; ++i) x[i] = (x[i - 1] + 1) % 50;
        CC_INSPECT(x[m - 1]);
    }

    const int n = 1000;
    const int m = 10;

    Local<Array<int>> test{n};
    for (int i = 0; i < n; ++i)
        test->at(i) = i;

    Local<Random>{0}->scramble(&test);
    // test->reverseInsitu();

    CC_INSPECT(sum(test));

    {
        auto ts = ::clock();
        for (int j = 0; j < m; ++j) {
            std::set<int> set;
            for (auto x: test) set.insert(x);
        }
        ts = ::clock() - ts;
        CC_INSPECT(ts);
    }
    {
        auto tl = ::clock();
        for (int j = 0; j < m; ++j) {
            Local<Set<int>> set;
            for (auto x: test) set->insert(x);
        }
        tl = ::clock() - tl;
        CC_INSPECT(tl);
    }
    {

        auto tx = ::clock();
        for (int j = 0; j < m; ++j) {
            Local<NextSet<int>> set;
            for (auto x: test) set->insert(x);
        }
        tx = ::clock() - tx;
        CC_INSPECT(tx);

        // CC_INSPECT(sum(set));

        #if 0
        for (int i = n - 1; i >= 0; --i) {
            set->remove(test[i]);
            // print(set);
        }
        #endif
    }

    return 0;
    #endif

    CC_TESTSUITE_ADD(AscendingInsertionRemovalTest);
    CC_TESTSUITE_ADD(DescendingInsertionRemovalTest);
    CC_TESTSUITE_ADD(RandomInsertionRemovalTest);

    return TestSuite::instance()->run(argc, argv);
}
