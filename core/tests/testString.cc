/*
 * Copyright (C) 2007-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/String>
#include <cc/Unicode>

using namespace cc;
using namespace cc::testing;

class ConstructionComparism: public TestCase
{
    void run()
    {
        String a = "Test!", b("1, 2, 3, ...");
        fout() << a << " " << b << nl;
        CC_VERIFY(a == "Test!");
        CC_VERIFY(a != "1, 2, 3, ...");
    }
};

class CountCopySplitJoin: public TestCase
{
    void run()
    {
        String s = "Übertragung";
        CC_VERIFY(s != "123" && s == "Übertragung");
        CC_VERIFY("X" < s);

        fout("s = \"%%\"\n") << s;
        fout("Unicode::open(s)->count() = %%\n") << Unicode::open(s)->count();
        fout("s->size() = %%\n") << s->count();
        fout("s->copy() = \"%%\"\n") << s->copy();

        Ref<StringList> parts = s->split("a");
        fout("s.split(\"a\") = [\n");
        for (int i = 0; i < parts->count(); ++i)
            fout("  \"%%\"\n") << parts->at(i);
        fout("]\n");
        CC_VERIFY(parts->join("a") == s);

        CC_VERIFY(String()->split("\n")->count() == 1);
    }
};

class UnicodeEscapes: public TestCase
{
    void run()
    {
        String s = "Hallo!, \n\\u041F\\u0440\\u0438\\u0432\\u0435\\u0442!, \\ud834\\udd22, Hello!";
        fout("s = \"%%\"\n") << s;
        String se = s->unescape();
        fout("Unicode::open(se)->at(17) = 0x%%\n") << hex(Unicode::open(se)->at(17), 2);
        fout("se = \"%%\"\n") << se;
    }
};

class FindSplitReplace: public TestCase
{
    void run() {
        {
            String s = "bin/testPath";
            // fout("s = \"%%\"\n") << s;
            fout("s->find(\"/\") = %%\n") << s->find("/");
            Ref<StringList> parts = s->split("/");
            fout("s.split(\"/\") = [\n");
            for (int i = 0; i < parts->count(); ++i)
                fout("  \"%%\"\n") << parts->at(i);
            fout("]\n");
        }
        {
            String s = "..Привет, Привет!";
            mutate(s)->replaceInsitu("Привет", "Hallo");
            fout("s = \"%%\"\n") << s;
        }
    }
};

class SyntaxSugar: public TestCase
{
    void run()
    {
        {
            String s = "123";
            for (auto ch: s) fout() << ch << nl;
        }
        {
            String s = "Привет!";
            for (auto ch: Unicode::open(s)) fout() << ch << nl;
        }
        {
            String s = "ABC";
            CC_INSPECT(s);
            for (char &ch: mutate(s)) ch = toLower(ch);
            CC_INSPECT(s);
            CC_VERIFY(s == "abc");
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(ConstructionComparism);
    CC_TESTSUITE_ADD(CountCopySplitJoin);
    CC_TESTSUITE_ADD(UnicodeEscapes);
    CC_TESTSUITE_ADD(FindSplitReplace);
    CC_TESTSUITE_ADD(SyntaxSugar);

    return TestSuite::instance()->run(argc, argv);
}
