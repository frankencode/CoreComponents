#include <ftl/stdio>
#include <ftl/strings>

using namespace ftl;

int main() {
	Pattern p = "*.(t[..]t|a[a..z]#)";
	String s = "abc.txt";
	Ref<Token, Owner> token = p.match(s);
	print("p.match(\"%%\") = 0x%hex%\n", s, (void*)token);
	if (token)
		print("i0, i1 = %%, %%\n", token->i0(), token->i1());
	#ifndef NDEBUG
	if (p.debugger())
		p.debugger()->printDefinition();
	#endif
	return 0;
}
