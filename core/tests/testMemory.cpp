#include <ftl/stdio>

using namespace ftl;

int main(int argc, char **argv)
{
	for (int j = 0; j < 3; ++j) {
		const int n = 10;
		void *p[n];
		for (int i = 0; i < n; ++i)
			p[i] = (void *) new char[i];
		for (int i = 0; i < n; ++i)
			print("%hex%, %%, %%, %%, %%\n", p[i], (uint64_t)p[i] % 4, (uint64_t)p[i] % 8, (uint64_t)p[i] % 16, (uint64_t)p[i] % 32);
	}
	return 0;
}
