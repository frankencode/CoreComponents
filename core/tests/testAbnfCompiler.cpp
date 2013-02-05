#include <ftl/PrintDebug.hpp>
#include <ftl/AbnfCompiler.hpp>
#include <ftl/SyntaxDebugger.hpp>
#include <ftl/File.hpp>

namespace ftl
{

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; ++i) {
		string text = File::open(argv[i], File::Read)->readAll();
		hook<AbnfCompiler> compiler = AbnfCompiler::create();
		hook<SyntaxDebugger> debugger = SyntaxDebugger::create();
		hook<SyntaxDefinition> definition = compiler->compile(text, debugger);
		debugger->printDefinition(false/*omitUnusedRules*/);
	}
	return 0;
}

} // namespace ftl

int main(int argc, char **argv)
{
	return ftl::main(argc, argv);
}
