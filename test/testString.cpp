#include <pona/Core.hpp>

namespace pona
{

int main()
{
	{
		String s("Hello!");
		output()->write(format("(#s, s) = (%%, \"%%\")\n") % s.length() % s);
		s.insert(5, ", world");
		output()->write(format("%%\n") % s);
		if (s.copy(0, 5) == "Hello")
		{
			int pos = s.find("world");
			if (pos != -1)
			{
				s.remove(pos, String("world").length());
				s.insert(pos, "echo");
				output()->write(format("%%\n") % s);
			}
		}
	}
	
	{
		String f = String("a").append(String("b"));
		f.append(String("="));
		f.append(Char('c'));
		output()->write(format("s=\"%%\"\n") % f);
	}
	
	// output()->write(format("sizeof(wchar_t) = %%\n") % sizeof(wchar_t));
	// output()->write(format("sizeof(pona::Char) = %%\n") % sizeof(Char));
	
#ifdef PONA_WINDOWS
	output()->write("\nPress <RETURN> to continue...\n");
	input()->readLine();
#endif
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}

