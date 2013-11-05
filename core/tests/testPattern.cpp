#include <flux/stdio.h>
#include <flux/check.h>
#include <flux/Pattern.h>

using namespace flux;

int main() {
	Pattern p = "^#{}.(?<=.)(?<!hello)(t[..]{?}|(?a:#)[^abc](?&a))$";
	#ifndef NDEBUG
	if (p->debugFactory())
		cast<SyntaxDebugger>(p->debugFactory())->printDefinition();
	#endif
	String s = "abc.txt";
	Ref<Token> token = p->match(s);
	fout("p->match(\"%%\") = 0x%%\n") << s << str((void *)token);
	check(token);
	fout("i0, i1 = %%, %%\n") << token->i0() << token->i1();
	return 0;
}
