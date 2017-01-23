/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/regexp/RegExp>

using namespace cc;
using namespace cc::regexp;
using namespace cc::testing;

class EmailValidation: public TestCase
{
    void run()
    {
        RegExp pattern =
            "(?>!:[.-])"
            "{1..:(?>!:..|--)([a..z]|[A..Z]|[0..9]|[.-])}"
            "(?<!:[.-])"
            "@"
            "(?>!:[.-])"
            "{1..:(?>!:..|--)([a..z]|[A..Z]|[0..9]|[.-])}"
            "(?<!:[.-])";

        #ifndef NDEBUG
        SyntaxDebugger *debugger = Object::cast<SyntaxDebugger *>(pattern->debugFactory());
        debugger->printDefinition();
        fout() << nl;
        #endif

        Ref<StringList> address = StringList::create()
            << "info@cyblogic.com"
            << "@lala.de"
            << "otto@übertragungsfehler.de"
            << ""
            << "a@B"
            << "a--b@C"
            << "a-b.@C"
            << "b-@C"
            << "a.b.c@d.e.f";

        Ref< List<bool> > result = List<bool>::create()
            << true
            << false
            << false
            << false
            << true
            << false
            << false
            << false
            << true;

        for (int i = 0; i < address->count(); ++i) {
            bool valid = pattern->match(address->at(i))->valid();
            fout() << i << ": \"" << address->at(i) << "\": " << valid << nl;
            CC_VERIFY(valid == result->at(i));
        }

        fout() << nl;
    }
};

class Globbing: public TestCase
{
    void run()
    {
        RegExp pattern = "*.(c|h){..2:[^.]}";

        Ref<StringList> path = StringList::create()
            << "/home/hans/glück.hh"
            << "a.b.c"
            << "a.b.c.d.e.f"
            << "test.cpp"
            << "std.hxx"
            << "stdio.h";

        #ifndef NDEBUG
        SyntaxDebugger *debugger = Object::cast<SyntaxDebugger *>(pattern->debugFactory());
        debugger->printDefinition();
        fout() << nl;
        #endif

        Ref< List<bool> > result = List<bool>::create()
            << true
            << true
            << false
            << true
            << true
            << true;

        for (int i = 0; i < path->count(); ++i) {
            bool valid = pattern->match(path->at(i))->valid();
            fout() << i << ": \"" << path->at(i) << "\": " << valid << nl;
            CC_VERIFY(valid == result->at(i));
        }

        fout() << nl;
    }
};

class LazyChoice: public TestCase
{
    void run()
    {
        RegExp pattern = "(Hans|HansPeter)Glück";
        CC_VERIFY(pattern->match(String("HansPeterGlück"))->valid());
        CC_VERIFY(pattern->match(String("HansGlück"))->valid());
    }
};

class UriDispatch: public TestCase
{
    void run()
    {
        Ref<StringList> services = StringList::create()
            << "glibc.*"
            << "cckit.*"
            << "books.*"
            << "test.*"
            << "*httpecho*";

        for (int i = 0; i < services->count(); ++i)
            fout() << services->at(i) << ": " << RegExp(services->at(i))->match(String("cckit.cyblogic.com"))->valid() << nl;
    }
};

#if 0
class TestEmpty: public TestCase
{
    void run()
    {
        fout() << RegExp(0)->match(String("Something"))->valid() << nl;
    }
};
#endif

int main(int argc, char** argv)
{
    CC_TESTSUITE_ADD(EmailValidation);
    CC_TESTSUITE_ADD(Globbing);
    CC_TESTSUITE_ADD(LazyChoice);
    CC_TESTSUITE_ADD(UriDispatch);

    return TestSuite::instance()->run(argc, argv);
}
