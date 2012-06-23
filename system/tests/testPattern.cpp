#include <ftl/stdio>
#include <ftl/strings>

using namespace ftl;

int main() {
	Pattern p = "^#{}.(?<=.)(?<!hello)(t[..]{?}|a[^abc]#)$";
	#ifndef NDEBUG
	if (p.debugger())
		p.debugger()->printDefinition();
	#endif
	String s = "abc.txt";
	Ref<Token, Owner> token = p.match(s);
	print("p.match(\"%%\") = 0x%hex%\n", s, (void*)token);
	if (token)
		print("i0, i1 = %%, %%\n", token->i0(), token->i1());
	return 0;
}
