/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/syntax/RegExp>

using namespace flux;
using namespace flux::testing;

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
        SyntaxDebugger *debugger = cast<SyntaxDebugger>(pattern->debugFactory());
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
            FLUX_VERIFY(valid == result->at(i));
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
        SyntaxDebugger *debugger = cast<SyntaxDebugger>(pattern->debugFactory());
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
            FLUX_VERIFY(valid == result->at(i));
        }

        fout() << nl;
    }
};

class LazyChoice: public TestCase
{
    void run()
    {
        RegExp pattern = "(Hans|HansPeter)Glück";
        FLUX_VERIFY(pattern->match(String("HansPeterGlück"))->valid());
        FLUX_VERIFY(pattern->match(String("HansGlück"))->valid());
    }
};

class UriDispatch: public TestCase
{
    void run()
    {
        Ref<StringList> services = StringList::create()
            << "glibc.*"
            << "fluxkit.*"
            << "books.*"
            << "test.*"
            << "*httpecho*";

        for (int i = 0; i < services->count(); ++i)
            fout() << services->at(i) << ": " << RegExp(services->at(i))->match(String("fluxkit.cyblogic.com"))->valid() << nl;
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
    FLUX_TESTSUITE_ADD(EmailValidation);
    FLUX_TESTSUITE_ADD(Globbing);
    FLUX_TESTSUITE_ADD(LazyChoice);
    FLUX_TESTSUITE_ADD(UriDispatch);

    return testSuite()->run(argc, argv);
}
