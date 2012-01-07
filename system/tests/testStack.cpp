#include <ftl/streams>
#include <ftl/container>
#include <ftl/utils>

namespace ftl
{

int primeCountSimple(int n) {
	int m = 0;
	for (int i = 2; i < n; ++i) {
		bool prime = true;
		for (int j = 2; (j < i) && prime; ++j)
			prime = prime && (i % j != 0);
		if (prime) {
			// print("%%: %%\n", m, i);
			++m;
		}
	}
	return m;
}

int primeCountWithStack(int n) {
	Stack<int> stack(n);
	for (int i = 2; i < n; ++i) {
		bool prime = true;
		int m = stack.fill();
		for (int j = 0; (j < m) && prime; ++j)
			prime = prime && (i % stack.bottom(j) != 0);
		if (prime) {
			// print("%%: %%\n", m, i);
			stack.push(i);
		}
	}
	return stack.fill();
}

int main()
{
	print("+---------------------------------+\n");
	print("| Prime Counting Performance Test |\n");
	print("+---------------------------------+\n");
	print("\n");
	print("Reference System: Laptop with Intel Core2\n");
	int n = 10000;
	print("Prime Count Range: [%%, %%]\n", 0, n);
	print("\n");
	{
		int usecRef = 40653;
		Time dt = Time::now();
		int m = primeCountSimple(n);
		int usec = (Time::now() - dt).us();
		print("primeCountSimple(): %% prime numbers, dt = %% us (%% % of reference system)\n", m, usec, (100 * usecRef) / usec);
	}
	{
		int usecRef = 5952;
		Time dt = Time::now();
		int m = primeCountWithStack(n);
		int usec = (Time::now() - dt).us();
		print("primeCountWithStack(): %% prime numbers, dt = %% us (%% % of reference system)\n", m, usec, (100 * usecRef) / usec);
	}
	print("\n");
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
