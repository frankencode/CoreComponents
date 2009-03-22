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
	
	// output()->write(format("sizeof(wchar_t) = %%\n") % sizeof(wchar_t));
	// output()->write(format("sizeof(pona::Char) = %%\n") % sizeof(Char));
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}

