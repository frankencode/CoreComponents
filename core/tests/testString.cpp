#include <fkit/stdio.h>
#include <fkit/String.h>

using namespace fkit;

int main()
{
	{
		String s = "Übertragung";
		fout("s = \"%%\"\n") << s;
		fout("s->chars()->length() = %%\n") << s->chars()->length();
		fout("s->length() = %%\n") << s->length();
		fout("s->copy() = \"%%\"\n") << s->copy();
		fout("String(s->chars()->copy(s->chars()->length()-3, s->chars()->length())) = \"%%\"\n") << String(s->chars()->copy(s->chars()->length()-3, s->chars()->length()));
		for (int i = 0; i < s->chars()->length(); ++i)
			fout("s[%%] = '%%' (%%)\n") << i << s->chars()->copy(i, i + 1) << s->chars()->get(i);
		fout("s->chars()->copy(s->chars()->length()-3, s->chars()->length()) = \"%%\"\n") << s->chars()->copy(s->chars()->length()-3, s->chars()->length());
		Ref<StringList> parts = s->split("a");
		fout("s.split(\"a\") = [\n");
		for (int i = 0; i < parts->length(); ++i)
			fout("  \"%%\"\n") << parts->get(i);
		fout("]\n");
	}
	{
		String s = "bin/testPath";
		// fout("s = \"%%\"\n") << s;
		fout("s->find(\"/\") = %%\n") << s->find("/");
		Ref<StringList> parts = s->split("/");
		fout("s.split(\"/\") = [\n");
		for (int i = 0; i < parts->length(); ++i)
			fout("  \"%%\"\n") << parts->get(i);
		fout("]\n");
	}
	{
		String s = "Hallo!, \n\\u041F\\u0440\\u0438\\u0432\\u0435\\u0442!, \\ud834\\udd22, Hello!";
		fout("s = \"%%\", s->chars()->length() = %%\n") << s << s->chars()->length();
		String se = s->expand();
		fout("se.get(16) = 0x%%\n") << hex(se->chars()->at(17), 2);
		fout("se = \"%%\", s->chars()->length() = %%\n") << se << se->chars()->length();
	}
	{
		String s = "..Привет, Привет!";
		s->replaceInsitu("Привет", "Hallo");
		fout("s = \"%%\"\n") << s;
	}
	return 0;
}
