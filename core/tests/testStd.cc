/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <algorithm>

using namespace cc;
using namespace cc::testing;

class TestAlgorithms: public TestCase
{
    void print(const List<String> *l) {
        for (auto x: l)
            fout() << "\"" << x << "\" ";
        fout() << nl;
    }

    void run()
    {
        auto fruits = List<String>::create() << "melon" << "banana" << "pear" << "melon" << "apple" << "peach";
        std::sort(fruits->begin(), fruits->end());
        print(fruits);
        {
            List<String>::const_iterator i = std::find(fruits->begin(), fruits->end(), "pear");
            fout() << "Index of " << *i << " is " << i - fruits->begin() << nl;
            CC_VERIFY(*i == "pear");
            CC_VERIFY(i - fruits->begin() == 5);
        }
        {
            int n = std::count(fruits->begin(), fruits->end(), "melon");
            fout() << "Number of melons: " << n << nl;
            CC_VERIFY(n == 2);
        }
        std::reverse(fruits->begin(), fruits->end());
        print(fruits);
        std::unique(fruits->begin(), fruits->end());
        print(fruits);
        {
            int n = std::count(fruits->begin(), fruits->end(), "melon");
            fout() << "Number of melons: " << n << nl;
            CC_VERIFY(n == 1);
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(TestAlgorithms);

    return TestSuite::instance()->run(argc, argv);
}
