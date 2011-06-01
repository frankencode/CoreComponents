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
	return 0;
}

} // namespace ftl

int main(int argc, char** argv)
{
	return ftl::main(argc, argv);
}
