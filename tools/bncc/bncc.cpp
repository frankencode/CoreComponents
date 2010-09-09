#include <ftl/stdio>
#include "use_ftl"
#include "AbnfCompiler.hpp"

namespace bncc
{

int main(int argc, char** argv)
{
	CommandLine commandLine;
	commandLine.summary(
		"Compiles an ABNF (RFC2234) grammar into an executable ftl::Syntax grammar."
	);
	commandLine.entity("FILE");
	Ref<CommandOption> verify  = commandLine.define('y', "verify",  false, "Verify source grammar only");
	Ref<CommandOption> help  = commandLine.define('h', "help",  false, "Print help");

	Ref<StringList, Owner> listOfFiles = commandLine.read(argc, argv);
	
	if (help->value()) {
		print(commandLine.helpText());
		return 0;
	}
	
	while (listOfFiles->hasNext())
	{
		File file(listOfFiles->next());
		file.open(File::Read);
		String sourceText = String::uninitialized(file.size());
		file.read(sourceText->data(), sourceText->size());
		file.close();
		
		AbnfCompiler compiler;
		compiler.compile(sourceText.media());
	}
	
	return 0;
}

} // namespace bncc

int main(int argc, char** argv)
{
	return bncc::main(argc, argv);
}
