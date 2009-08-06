#include <pona/stdio>

namespace pona
{

int main()
{
	{
		String s("Hello!");
		print("(#s, s) = (%%, \"%%\")\n", s->length(), s);
		s->insert(5, String(", world"));
		print("(#s, s) = (%%, \"%%\")\n", s->length(), s);
		if (s->copy(0, 5) == "Hello") {
			int pos = s->find(String("world"));
			print("pos = %%\n", pos);
			if (pos != s->length())
			{
				s->remove(pos, String("world")->length());
				s->insert(pos, String("echo"));
				print("%%\n", s);
			}
		}
	}
	{
		String f = String("a") << "b";
		f->append(String("="));
		f->append(Char('c'));
		print("s=\"%%\"\n", f);
	}
	
	{
		print("\"123\" < \"12\" = %%\n", String("123") < String("12"));
		print("\"12\" < \"123\" = %%\n", String("12") < String("123"));
		print("\"123\" == \"12\" = %%\n", String("123") == String("12"));
		print("\"ab\" < \"ab\" = %%\n", String("ab") < String("ab"));
		print("\"ab\" == \"ab\" = %%\n", String("ab") == String("ab"));
		print("\"\" < \"\" = %%\n", String() < String());
		print("\"\" == \"\" = %%\n", String() == String());
	}
	
	{
		print("\n");
		Ref<StringList, Owner> sl0 = String("AB,0123,lol") / String(",");
		Ref<StringList, Owner> sl1 = String("AB,0123,lol,") / String(",");
		Ref<StringList, Owner> sl2 = String("") / String(",");
		Ref<StringList, Owner> sl3 = String(",") / String(",");
		Ref<StringList, Owner> sl4 = String(",,a,,") / String(",,");
		for (int i = 0, n = sl0->length(); i < n; ++i)
			print("sl0->get(%%) = \"%%\"\n", i, sl0->get(i));
		print("\n");
		for (int i = 0, n = sl1->length(); i < n; ++i)
			print("sl1->get(%%) = \"%%\"\n", i, sl1->get(i));
		print("\n");
		for (int i = 0, n = sl2->length(); i < n; ++i)
			print("sl2->get(%%) = \"%%\"\n", i, sl2->get(i));
		print("\n");
		for (int i = 0, n = sl3->length(); i < n; ++i)
			print("sl3->get(%%) = \"%%\"\n", i, sl3->get(i));
		print("\n");
		for (int i = 0, n = sl4->length(); i < n; ++i)
			print("sl4->get(%%) = \"%%\"\n", i, sl4->get(i));
	}
	// output()->write(format("sizeof(wchar_t) = %%\n") % sizeof(wchar_t));
	// output()->write(format("sizeof(pona::Char) = %%\n") % sizeof(Char));
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}

