#include <pona/stdio>

namespace pona
{

int main(int argc, char** argv)
{
	{
		LineParser parser("-100, -123.456e-9, 'trara\nlalal'", LineParser::DefaultOptions | LineParser::Strict);
		int i = parser.readInt();
		double d = parser.readDouble();
		String s = parser.readString();
		output()->writeLine(format("%%, %%, \"%%\"") % i % d % s);
	}
	
	{
		double a, b, c;
		bool f1, f2;
		LineParser parser("-1; 2.; 3e3; true; false; smth");
		String s;
		if (parser % a % b % c % f1 % f2 % s)
			output()->writeLine(format("%%; %%; %%; %%; %%; %%") % a % b % c % f1 % f2 % s);
		else 
			output()->write(format("error: parser.reason() = \"%%\"\n") % parser.reason());
	}
	
	{
		double a, b;
		String op;
		LineParser parser("1 - 2", LineParser::AutoSkipSpace);
		
		if (parser % a % op % b)
		{
			if (op == "+")
				output()->writeLine(format("%% + %% = %%") % a % b % (a + b));
			else if (op == "-")
				output()->writeLine(format("%% - %% = %%") % a % b % (a - b));
			else
				output()->writeLine("Unsupported operator.");
		}
		else
			output()->write(format("error: parser.reason() = \"%%\"\n") % parser.reason());
	}
	
	return 0;
}

} // namespace pona

int main(int argc, char** argv)
{
	return  pona::main(argc, argv);
}
