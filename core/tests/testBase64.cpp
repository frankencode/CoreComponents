#include <flux/stdio.h>
#include <flux/check.h>
#include <flux/Base64.h>

using namespace flux;

int main()
{
	String test[] = { "Man", "Hello world", "", "1" };
	const int testCount = sizeof(test) / sizeof(test[0]);

	for (int i = 0; i < testCount; ++i) {
		String a = test[i], b = Base64::encode(a);
		fout("base64(\"%%\") = \"%%\"\n") << a << b;
		check(Base64::decode(b) == a);
	}

	return 0;
}
