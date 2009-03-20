#include <pona/stdio>
#include <pona/container>

namespace pona
{

int main()
{
	{
		output()->write("Test 1\n");
		output()->write("------\n");
		List<int> list;
		for (int i = 0; i < 10; ++i)
			list.pushBack(i);
		for (int i = 0; i < list.fill(); ++i)
			output()->write(format("list[%%]=%%\n") % i % list.get(i));
		output()->write("--\n");
		for (int i = 0; i < list.fill(); ++i)
			list.pop(i);
		for (int i = 0; i < list.fill(); ++i)
			output()->write(format("list[%%]=%%\n") % i % list.get(i));
		output()->write("\n");
	}
	
	{
		output()->write("Test 2\n");
		output()->write("------\n");
		
		const char* digits = "0123456789";
		List<char> list;
		for (int i = 0; i < 10; ++i)
			list.pushBack(digits[i]);
		for (int i = 0; i < list.fill(); ++i)
			output()->write(format("list[%%]=%%\n") % i % list.get(i));
		
		output()->write("\n");
	}
	
	{
		output()->write("Test 3\n");
		output()->write("------\n");
		
		const char* l1 = "#include <pona/atoms>\n";
		const char* l2 = "int main() { pona::ouput()->write(\"Hello, world!\\n\"); return 0; }\n";
		List<char> text;
		text.push(0, strlen(l1), l1);
		text.push(text.fill(), strlen(l2), l2);
		for (int i = 0; i < text.fill(); ++i)
			output()->write(format("%%") % text.get(i));
		char buf[128];
		text.pop(60, 13, buf);
		for (int i = 0; i < text.fill(); ++i)
			output()->write(format("%%") % text.get(i));
		output()->write("\n--\n");
		
		const char* greeting = "U";
		text.push(60, 1, greeting);
		
		char buf2[128];
		memset(buf2, 0, 128);
		text.read(60, 13, buf2);
		output()->write(format("\"%%\"\n") % buf2);
	}
	
	{
		output()->write("Text 4\n");
		output()->write("------\n");
		List<Char> l;
		l.push(0, 2);
		l.set(0, '0');
		l.set(1, '\n');
		l.push(2, 2);
		l.set(2, '1');
		l.set(3, '\n');
		l.push(4, 2);
		l.set(4, '2');
		l.set(5, '\n');
		l.pop(0, 4);
		for (int i = 0; i < l.length(); ++i) {
			output()->write(format("l.get(%%) = %%\n") % i % l.get(i));
		}
	}
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
