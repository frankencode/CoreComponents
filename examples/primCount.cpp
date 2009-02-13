#include <pona/Core.hpp>

namespace pona
{

int primCountSimple(int n) {
	int m = 0;
	for (int i = 2; i < n; ++i) {
		bool prim = true;
		for (int j = 2; (j < i) && prim; ++j)
			prim = prim && (i % j != 0);
		if (prim) {
			// print("%%: %%\n", m, i);
			++m;
		}
	}
	return m;
}

int primCountWithStack(int n) {
	Stack<int> stack(n);
	for (int i = 2; i < n; ++i) {
		bool prim = true;
		int m = stack.fill();
		for (int j = 0; (j < m) && prim; ++j)
			prim = prim && (i % stack.bottom(j) != 0);
		if (prim) {
			// print("%%: %%\n", m, i);
			stack.push(i);
		}
	}
	return stack.fill();
}

int main()
{
	print("+--------------------------------+\n");
	print("| Prim Counting Performance Test |\n");
	print("+--------------------------------+\n");
	print("\n");
	print("Reference System: Laptop with Intel Core2\n");
	int n = 10000;
	print("Prim Count Range: [%%, %%]\n", 0, n);
	print("\n");
	{
		int usecRef = 40653;
		TimeStamp dt = getTime();
		int m = primCountSimple(n);
		int usec = (getTime() - dt).microSeconds();
		print("primtCountSimple(): %% prim numbers, dt = %% us (%% % of reference system)\n", m, usec, (100 * usecRef) / usec);
	}
	{
		int usecRef = 5952;
		TimeStamp dt = getTime();
		int m = primCountWithStack(n);
		int usec = (getTime() - dt).microSeconds();
		print("primCountWithStack(): %% prim numbers, dt = %% us (%% % of reference system)\n", m, usec, (100 * usecRef) / usec);
	}
	print("\n");
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
