#include <ftl/PrintDebug.hpp>
#include <ftl/String.hpp>

namespace ftl
{

void testRef1(String s)
{
	print("testRef1(): s->refCount() = %% (s = '%%')\n", s->refCount(), s);
}

void testRef2(const String &s)
{
	print("testRef2(): s->refCount() = %% (s = '%%')\n", s->refCount(), s);
}

inline void testRef3(String s)
{
	print("testRef3(): s->refCount() = %% (s = '%%')\n", s->refCount(), s);
}

inline void testRef4(ByteArray *s)
{
	print("testRef4(): s->refCount() == %% (s = '%%')\n", s->refCount(), s);
}

inline void testRef5(const String s)
{
	print("testRef5(): s->refCount() == %% (s = '%%')\n", s->refCount(), s);
}

void printIt(Ref<ByteArray> s)
{
	print("s = \"%%\"\n", s);
}

int main(int argc, char **argv)
{
	{
		typedef List< Ref<ByteArray> > Parts;
		Ref<Parts> parts = Parts::create();
		parts->append(ByteArray::copy("Hello!"));
		Ref<ByteArray> s = parts;
		print("s = \"%%\"\n", s);
		class Smth: public Parts { public: Smth() { append(ByteArray::copy("hi!")); } };
		Ref<Parts> parts2 = new Smth;
		printIt(parts2);
	}

	return 0;

	{
		String s = "Übertragung";
		print("s = \"%%\"\n", s);
		print("s->chars()->length() = %%\n", s->chars()->length());
		print("s->length() = %%\n", s->length());
		print("s->copy() = \"%%\"\n", s->copy());
		print("String(s->chars()->copy(s->chars()->length()-3, s->chars()->length())) = \"%%\"\n", String(s->chars()->copy(s->chars()->length()-3, s->chars()->length())));
		for (int i = 0; i < s->chars()->length(); ++i)
			print("s[%%] = '%%' (%%)\n", i, s->chars()->copy(i, i + 1), s->chars()->get(i));
		print("s->chars()->copy(s->chars()->length()-3, s->chars()->length()) = \"%%\"\n", s->chars()->copy(s->chars()->length()-3, s->chars()->length()));
		Ref<StringList> parts = s->split("a");
		print("s.split(\"a\") = [\n");
		for (int i = 0; i < parts->length(); ++i)
			print("  \"%%\"\n", parts->get(i));
		print("]\n");
	}
	{
		String s = "bin/testPath";
		// print("s = \"%%\"\n", s);
		print("s->find(\"/\") = %%\n", s->find("/"));
		Ref<StringList> parts = s->split("/");
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
		print("s = \"%%\", s->chars()->length() = %%\n", s, s->chars()->length());
		String se = s->expand();
		print("se.get(16) = %hex%\n", se->chars()->at(17));
		print("se = \"%%\", s->chars()->length() = %%\n", se, se->chars()->length());
	}
	{
		String s = "..Привет, Привет!";
		s->replaceInsitu("Привет", "Hello");
		print("s = \"%%\"\n", s);
	}
	{
		String s = "test 1, 2, 3...";
		testRef1(s);
		testRef2(s);
		testRef3(s);
		testRef4(s);
		testRef5(s);
	}
	return 0;
}

} // namespace ftl

int main(int argc, char **argv)
{
	return ftl::main(argc, argv);
}
