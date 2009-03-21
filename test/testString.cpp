#include <pona/stdio>

namespace pona
{

int main()
{
	{
		String s("Hello!");
		print("(#s, s) = (%%, \"%%\")\n", s->length(), s);
		s->insert(5, String(", world"));
		print("%%\n", s);
		if (s->copy(0, 5) == "Hello")
		{
			int pos = s.find("world");
			if (pos != -1)
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
	
	// output()->write(format("sizeof(wchar_t) = %%\n") % sizeof(wchar_t));
	// output()->write(format("sizeof(pona::Char) = %%\n") % sizeof(Char));
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}

