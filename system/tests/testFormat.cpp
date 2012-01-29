#include <ftl/stdio>
#include <ftl/utils>

namespace ftl
{

int main()
{
	{
		String s = Format() << 1 << ',' << " 2, " << 3.3 << ", " << &s;
		print("s = \"%%\"\n", s);
	}
	{
		Variant a = 1, b = true, c = "abc", d = 3.3;
		print(Format() << a << ", " << b << ", " << c << ", " << d << "\n");
	}
	{
		print(Format("(%hex%)16, (%oct%)8, (%bin%)2...\n") << 123 << 123 << 123.);
	}
	{
		const double x[] = {
			1., 0.1, 1.1, 0., 1.234e10, 1e-308, nan, inf 
			-1./3., -0.55, 0.49, 15, -1.5, 1.1111111111,
			1.1111111111111111111111111111111,
			1e-16
		};
		
		{
			String tmpl = "%dec:15:5.:'_'%\n"; // also to be tested: 5.5, 5.e, 10:5. ...etc
			
			for (int i = 0, n = sizeof(x) / sizeof(x[0]); i < n; ++i)
				print(Format(tmpl) << x[i]);
		}
	}
	/*{
		print("\n");
		Random random;
		for (int i = 0; i < 100; ++i) {
			double b = double(random.next()) / random.max();
			double e = random.next() % 200;
			double x = pow(b, e);
			print(Format("%.5e%") << x); printf(" = %g\n", x);
		}
	}*/
	
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
