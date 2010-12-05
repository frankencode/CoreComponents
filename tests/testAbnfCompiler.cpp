#include <ftl/streams>
#include <ftl/AbnfCompiler.hpp>
#include <ftl/SyntaxDebugger.hpp>

namespace ftl
{

int main(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i) {
		Ref<File, Owner> file = new File(argv[i]);
		file->open(File::Read);
		String text = file->readAll();
		Ref<AbnfCompiler, Owner> compiler = new AbnfCompiler;
		Ref<AbnfCompiler::Debugger, Owner> debugger = new AbnfCompiler::Debugger;
		Ref<AbnfCompiler::Definition, Owner> definition = compiler->compile(text, debugger);
		debugger->printDefinition();
	}
	return 0;
}

} // namespace ftl

int main(int argc, char** argv)
{
	return ftl::main(argc, argv);
}
