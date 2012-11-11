#include <ftl/stdio>
#include <ftl/AbnfCompiler.hpp>
#include <ftl/SyntaxDebugger.hpp>

namespace ftl
{

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; ++i) {
		String text = File::open(argv[i], File::Read)->readAll();
		Ref<AbnfCompiler, Owner> compiler = AbnfCompiler::newInstance();
		Ref<SyntaxDebugger, Owner> debugger = SyntaxDebugger::newInstance();
		Ref<SyntaxDefinition, Owner> definition = compiler->compile(text, debugger);
		debugger->printDefinition(false/*omitUnusedRules*/);
	}
	return 0;
}

} // namespace ftl

int main(int argc, char **argv)
{
	return ftl::main(argc, argv);
}
