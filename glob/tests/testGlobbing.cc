/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/glob/Pattern>

using namespace cc;
using namespace cc::glob;
using namespace cc::testing;

class EmailValidation: public TestCase
{
    void run()
    {
        Pattern pattern =
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
        Pattern pattern = "*.(c|h){..2:[^.]}";

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

class CxxPathGlobbing: public TestCase
{
    void run()
    {
        Pattern pattern = "*/include/*";
        CC_VERIFY(pattern->match("/usr/include/c++/5/list")->valid());
        CC_VERIFY(!pattern->match("/usr/lib/libnegative.so")->valid());
    }
};

class LazyChoice: public TestCase
{
    void run()
    {
        Pattern pattern = "(Hans|HansPeter)Glück";
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
            fout() << services->at(i) << ": " << Pattern(services->at(i))->match(String("cckit.cyblogic.com"))->valid() << nl;
    }
};

#if 0
class TestEmpty: public TestCase
{
    void run()
    {
        fout() << Pattern(0)->match(String("Something"))->valid() << nl;
    }
};
#endif

int main(int argc, char** argv)
{
    CC_TESTSUITE_ADD(EmailValidation);
    CC_TESTSUITE_ADD(Globbing);
    CC_TESTSUITE_ADD(CxxPathGlobbing);
    CC_TESTSUITE_ADD(LazyChoice);
    CC_TESTSUITE_ADD(UriDispatch);

    return TestSuite::instance()->run(argc, argv);
}
