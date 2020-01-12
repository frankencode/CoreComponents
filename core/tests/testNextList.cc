#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/NextList>
#include <time.h>
#include <list>
#include <cc/List>
#include <cc/File>
#include <cc/Random>

using namespace cc;
using namespace cc::testing;

void print(const NextList<int> *list)
{
    Format f;
    for (int i = 0; i < list->count(); ++i)
        f << list->at(i);
    ferr() << "(" << f->join(", ") << ")" << nl;
}

class PushBackPopBackTest: public TestCase, public ListTesting<int>
{
    void run() override
    {
        const int n = 1024;
        Local<NextList<int>> list;
        for (int i = 0; i < n; ++i) {
            list->push(list->count(), i);
            for (int k = 0; k <= i; ++k)
                CC_VERIFY(list->at(k) == k);
        }
        CC_VERIFY(list->count() == n);
        File::open("tree_push_back.dot", FileOpen::Create|FileOpen::WriteOnly|FileOpen::Truncate)->write(tree(list)->dotify());
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n - i; ++k)
                CC_VERIFY(list->at(k) == k);
            list->pop(list->count() - 1);
        }
        CC_VERIFY(list->count() == 0);
    }
};

class PushFrontPopFrontTest: public TestCase, public ListTesting<int>
{
    void run() override
    {
        const int n = 1024;
        Local<NextList<int>> list;
        for (int i = 0; i < n; ++i) {
            list->push(0, i);
            for (int k = 0; k <= i; ++k)
                CC_VERIFY(list->at(k) == i - k);
        }
        CC_VERIFY(list->count() == n);
        File::open("tree_push_front.dot", FileOpen::Create|FileOpen::WriteOnly|FileOpen::Truncate)->write(tree(list)->dotify());
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n - i; ++k)
                CC_VERIFY(list->at(k) == n - i - 1 - k);
            list->pop(0);
        }
        CC_VERIFY(list->count() == 0);
    }
};

class PushFrontPopBackTest: public TestCase, public ListTesting<int>
{
    void run() override
    {
        const int n = 1024;
        Local<NextList<int>> list;
        for (int i = 0; i < n; ++i) {
            list->push(0, i);
            for (int k = 0; k <= i; ++k)
                CC_VERIFY(list->at(k) == i - k);
        }
        CC_VERIFY(list->count() == n);
        // File::open("tree_push_front.dot", FileOpen::Create|FileOpen::WriteOnly|FileOpen::Truncate)->write(tree(list)->dotify());
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n - i; ++k)
                CC_VERIFY(list->at(k) == n - 1 - k);
            list->pop(list->count() - 1);
        }
        CC_VERIFY(list->count() == 0);
    }
};

class PushBackPopFrontTest: public TestCase, public ListTesting<int>
{
    void run() override
    {
        const int n = 1024;
        Local<NextList<int>> list;
        auto t = ::clock();
        for (int i = 0; i < n; ++i) {
            list->push(list->count(), i);
        }
        t = ::clock() - t;
        CC_INSPECT(t);
        CC_VERIFY(list->count() == n);
        // File::open("tree_push_back.dot", FileOpen::Create|FileOpen::WriteOnly|FileOpen::Truncate)->write(tree(list)->dotify());
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n - i; ++k)
                CC_VERIFY(list->at(k) == k + i);
            list->pop(0);
        }
        CC_VERIFY(list->count() == 0);
    }
};

class PushPopRandomTest: public TestCase, public ListTesting<int>
{
    void run() override
    {
        const int n = 1024;
        Local<NextList<int>> list;
        Local<Random> random{0};
        auto t = ::clock();
        for (int i = 0; i < n; ++i) {
            auto r = random->get(0, i + 1);
            list->push(r, i);
        }
        t = ::clock() - t;
        CC_INSPECT(t);
        CC_VERIFY(list->count() == n);
        File::open("tree_random.dot", FileOpen::Create|FileOpen::WriteOnly|FileOpen::Truncate)->write(tree(list)->dotify());
        for (int i = 0; i < n; ++i) {
            auto r = random->get(0, list->count() - 1);
            list->pop(r);
        }
        CC_VERIFY(list->count() == 0);
    }
};

int main(int argc, char **argv)
{
    #if 0
    Local<NextList<int>> list;
    for (int i = 0; i < 32; ++i) list->push(i, i);
    for (auto x: list) CC_INSPECT(x);
    return 0;
    #endif

    #if 1 // def NDEBUG
    const int n = 256;
    const int w = 1000;
    {
        const int m = 1000000;
        uint32_t x[m];
        for (int i = 1; i < m; ++i) x[i] = (x[i - 1] + 1) % 50;
        CC_INSPECT(x[m - 1]);
    }
    {
        Local<NextList<int>> list;
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
    {
        Local<List<int>> list;
        for (int i = 0; i < n; ++i) list->append(i);
        auto tl = ::clock();
        int h = 0;
        for (int i = 0; i < w; ++i) {
            for (int x: list) h += x;
        }
        tl = ::clock() - tl;
        CC_INSPECT(h);
        CC_INSPECT(tl);
    }
    return 0;
    #endif
    #if 0
    const int n = 65535;
    Local<NextList<int>> list;
    for (int i = 0; i < n; ++i)
        list->push(0, i);
    File::open("tree_push_front.dot", FileOpen::Create|FileOpen::WriteOnly|FileOpen::Truncate)->write(list->tree()->dotify());

    return 0;
    #endif

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
            Local<NextList<int>> list;
            for (int i = 0; i < n; ++i)
                list->push(i, i);
        }
        {
            Local<NextList<int>> list;
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
            Local<List<int>> list;
            for (int i = 0; i < n; ++i)
                list->insertAt(i, i);
        }
        {
            Local<List<int>> list;
            auto tl = ::clock();
            for (int i = 0; i < n; ++i)
                list->insertAt(i, i);
            tl = ::clock() - tl;
            CC_INSPECT(tl);
        }
        #endif

    }

    return 0;
    #endif

    #if 1 // ndef NDEBUG
    CC_TESTSUITE_ADD(PushBackPopBackTest);
    CC_TESTSUITE_ADD(PushFrontPopFrontTest);
    CC_TESTSUITE_ADD(PushFrontPopBackTest);
    CC_TESTSUITE_ADD(PushBackPopFrontTest);
    CC_TESTSUITE_ADD(PushPopRandomTest);

    return TestSuite::instance()->run(argc, argv);
    #endif
}
