#include <ftl/streams>
#include <ftl/strings>

namespace ftl
{

int main(int argc, char** argv)
{
	{
		String s = "Übertragung";
		print("s = \"%%\"\n", s);
		print("s.length() = %%\n", s.length());
		print("s->length() = %%\n", s->length());
		print("s.copy() = \"%%\"\n", s.copy());
		print("String(s.character()->copy(s.length()-3, s.length())) = \"%%\"\n", String(s.character()->copy(s.length()-3, s.length())));
		for (int i = 0; i < s.length(); ++i)
			print("s[%%] = '%%' (%%)\n", i, s.copy(i, i + 1), s[i]);
		print("s.copy(s.length()-3, s.length()) = \"%%\"\n", s.copy(s.length()-3, s.length()));
		Ref<StringList, Owner> parts = s.split("a");
		print("s.split(\"a\") = [\n");
		for (int i = 0; i < parts->length(); ++i)
			print("  \"%%\"\n", parts->get(i));
		print("]\n");
	}
	{
		String s = "bin/testPath";
		// print("s = \"%%\"\n", s);
		print("s.find(\"/\") = %%\n", s.find(0, "/"));
		Ref<StringList, Owner> parts = s.split("/");
		print("s.split(\"/\") = [\n");
		for (int i = 0; i < parts->length(); ++i)
			print("  \"%%\"\n", parts->get(i));
		print("]\n");
	}
	{
		/*String s = "Привет!";
		for (int i = 0; i < s.length(); ++i)
			print("%% (\\u%hex:4:4.:'0'%)\n", s.copy(i, i + 1), s.get(i));*/
		String s = "Hallo!, \n\\u041F\\u0440\\u0438\\u0432\\u0435\\u0442!, \\ud834\\udd22, Hello!";
		print("s = \"%%\", s.length() = %%\n", s, s.length());
		String se = s.expand();
		print("se.get(16) = %hex%\n", se.get(17));
		print("se = \"%%\", s.length() = %%\n", se, se.length());
	}
	{
		String s = "..Привет, Привет!";
		s.replaceInsitu("Привет", "Hello");
		print("s = \"%%\"\n", s);
	}
	return 0;
}

} // namespace ftl

int main(int argc, char** argv)
{
	return ftl::main(argc, argv);
}
