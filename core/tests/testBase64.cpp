#include <flux/stdio.h>
#include <flux/check.h>
#include <flux/base64.h>

using namespace flux;

int main()
{
	String test[] = { "Man", "Hello world", "", "1" };
	const int testCount = sizeof(test) / sizeof(test[0]);

	for (int i = 0; i < testCount; ++i) {
		String a = test[i], b = base64::encode(a);
		fout("base64(\"%%\") = \"%%\"\n") << a << b;
		check(base64::decode(b) == a);
	}

	return 0;
}
