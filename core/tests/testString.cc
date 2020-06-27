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
#include <cc/Casefree>

using namespace cc;
using namespace cc::testing;

class ArgumentPassing: public TestCase
{
    void byValue(String s) {
        CC_INSPECT(s->refCount());
    }

    void byConstValue(const String s) {
        CC_INSPECT(s->refCount());
    }

    void byConstRef(const String &s) {
        CC_INSPECT(s->refCount());
    }

    void run() override
    {
        String a{"123"};
        byValue(a);
        byConstValue(a);
        byConstRef(a);
        byValue("123");
        byConstRef("123");
    }
};

class ConstructionComparism: public TestCase
{
    void run() override
    {
        String a = "Test!", b{"1, 2, 3, ..."};
        fout() << a << " " << b << nl;
        CC_VERIFY(a == "Test!");
        CC_VERIFY(a != "1, 2, 3, ...");
    }
};

class CountCopySplitJoin: public TestCase
{
    void run() override
    {
        String s = "Übertragung";
        CC_VERIFY(s != "123" && s == "Übertragung");
        CC_VERIFY("X" < s);

        fout("s = \"%%\"\n") << s;
        fout("count(Unicode{s}) = %%\n") << count(Unicode{s});
        fout("s->size() = %%\n") << s->count();
        fout("s->copy() = \"%%\"\n") << s->copy();

        auto parts = s->split("a");
        fout("s.split(\"a\") = [\n");
        for (int i = 0; i < parts->count(); ++i)
            fout("  \"%%\"\n") << parts->at(i);
        fout("]\n");
        CC_VERIFY(parts->join("a") == s);

        CC_VERIFY(String{}->split("\n")->count() == 1);
    }
};

class UnicodeEscapes: public TestCase
{
    void run() override
    {
        String s = "\\u041F\\u0440\\u0438\\u0432\\u0435\\u0442!, \\ud834\\udd22";
        fout("s = \"%%\"\n") << s;
        String se = s->unescape();
        fout("se = \"%%\"\n") << se;
        for (auto ch: Unicode{se})
            fout() << ch << nl;
    }
};

class FindSplitReplace: public TestCase
{
    void run() override
    {
        {
            String s = "bin/testPath";
            // fout("s = \"%%\"\n") << s;
            fout("s->scan(\"/\") = %%\n") << s->scan("/");
            auto parts = s->split("/");
            fout("s.split(\"/\") = [\n");
            for (auto part: parts)
                fout("  \"%%\"\n") << part;
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
    void run() override
    {
        {
            String s = "123";
            for (auto ch: s) fout() << ch << nl;
        }
        {
            String s = "Привет!";
            for (auto ch: Unicode{s}) fout() << ch << nl;
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

class CasefreeComparisms: public TestCase
{
    void run() override
    {
        CC_VERIFY(Casefree<String>{"Hello"} == Casefree<String>{"hello"});
        CC_VERIFY(Casefree<String>{"abra"} < Casefree<String>{"Kadabra"});
        CC_VERIFY(Casefree<String>{"abra"} < Casefree<String>{"ABRAKADABRA"});
        CC_VERIFY(Casefree<String>{"abra"} != Casefree<String>{"abrakadabra"});
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(ArgumentPassing);
    CC_TESTSUITE_ADD(ConstructionComparism);
    CC_TESTSUITE_ADD(CountCopySplitJoin);
    CC_TESTSUITE_ADD(UnicodeEscapes);
    CC_TESTSUITE_ADD(FindSplitReplace);
    CC_TESTSUITE_ADD(SyntaxSugar);
    CC_TESTSUITE_ADD(CasefreeComparisms);

    return TestSuite::instance()->run(argc, argv);
}
