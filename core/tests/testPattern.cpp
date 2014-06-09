#include <flux/stdio.h>
#include <flux/check.h>
#include <flux/Pattern.h>

using namespace flux;

void testEmailValidation()
{
	Pattern pattern =
		"(!>:[.-])"
		"{1..:(!>:..|--)([a..z]|[A..Z]|[0..9]|[.-])}"
		"(!<:[.-])"
		"@"
		"(!>:[.-])"
		"{1..:(!>:..|--)([a..z]|[A..Z]|[0..9]|[.-])}"
		"(!<:[.-])";

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
		check(valid == result->at(i));
	}

	fout() << nl;
}

void testGlobbing()
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
		check(valid == result->at(i));
	}

	fout() << nl;
}

int main()
{
	testEmailValidation();
	testGlobbing();
	return 0;
}
