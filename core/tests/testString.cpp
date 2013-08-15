#include <fkit/stdio.h>
#include <fkit/String.h>
#include <fkit/Unicode.h>

using namespace fkit;

int main()
{
	{
		String s = "Übertragung";
		fout("s = \"%%\"\n") << s;
		fout("Unicode::open(s)->count() = %%\n") << Unicode::open(s)->size();
		fout("s->size() = %%\n") << s->size();
		fout("s->copy() = \"%%\"\n") << s->copy();

		Ref<StringList> parts = s->split("a");
		fout("s.split(\"a\") = [\n");
		for (int i = 0; i < parts->size(); ++i)
			fout("  \"%%\"\n") << parts->at(i);
		fout("]\n");
	}
	{
		String s = "bin/testPath";
		// fout("s = \"%%\"\n") << s;
		fout("s->find(\"/\") = %%\n") << s->find("/");
		Ref<StringList> parts = s->split("/");
		fout("s.split(\"/\") = [\n");
		for (int i = 0; i < parts->size(); ++i)
			fout("  \"%%\"\n") << parts->at(i);
		fout("]\n");
	}
	{
		String s = "Hallo!, \n\\u041F\\u0440\\u0438\\u0432\\u0435\\u0442!, \\ud834\\udd22, Hello!";
		fout("s = \"%%\"\n") << s;
		String se = s->expand();
		fout("Unicode::open(se)->at(17) = 0x%%\n") << hex(Unicode::open(se)->at(17), 2);
		fout("se = \"%%\"\n") << se;
	}
	{
		String s = "..Привет, Привет!";
		s->replaceInsitu("Привет", "Hallo");
		fout("s = \"%%\"\n") << s;
	}
	return 0;
}
