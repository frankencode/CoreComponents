#include <ftl/PrintDebug.hpp>
#include <ftl/Pattern.hpp>

using namespace ftl;

int main() {
	Pattern p = "^#{}.(?<=.)(?<!hello)(t[..]{?}|(?a:#)[^abc](?&a))$";
	#ifndef NDEBUG
	if (p->debugFactory())
		cast<SyntaxDebugger>(p->debugFactory())->printDefinition();
	#endif
	String s = "abc.txt";
	Ref<Token> token = p->match(s);
	print("p->match(\"%%\") = 0x%hex%\n", s, (void *)token);
	if (token)
		print("i0, i1 = %%, %%\n", token->i0(), token->i1());
	return 0;
}
