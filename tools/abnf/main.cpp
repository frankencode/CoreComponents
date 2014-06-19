#include <flux/stdio.h>
#include <flux/SyntaxDebugger.h>
#include <flux/abnf/AbnfCompiler.h>

using namespace flux;
using namespace flux::abnf;

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; ++i) {
		String text = File::open(argv[i])->map();
		Ref<AbnfCompiler> compiler = AbnfCompiler::create();
		Ref<SyntaxDebugger> debugger = SyntaxDebugger::create();
		Ref<SyntaxDefinition> definition = compiler->compile(text, debugger);
		debugger->printDefinition(false/*omitUnusedRules*/);
	}

	return 0;
}
