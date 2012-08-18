#include <ftl/stdio>
#include <ftl/AbnfCompiler.hpp>
#include <ftl/SyntaxDebugger.hpp>

namespace ftl
{

int main(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i) {
		Ref<File, Owner> file = File::newInstance(argv[i]);
		file->open(File::Read);
		String text = file->readAll();
		Ref<AbnfCompiler, Owner> compiler = AbnfCompiler::newInstance();
		Ref<SyntaxDebugger, Owner> debugger = SyntaxDebugger::newInstance();
		Ref<SyntaxDefinition, Owner> definition = compiler->compile(text, debugger);
		debugger->printDefinition(false/*omitUnusedRules*/);
	}
	return 0;
}

} // namespace ftl

int main(int argc, char** argv)
{
	return ftl::main(argc, argv);
}
