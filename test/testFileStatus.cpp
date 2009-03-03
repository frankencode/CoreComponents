#include <pona/Core.hpp>

namespace pona
{

void printStatus(String path)
{
	FileStatus stat(path);

	output()->write(format("FileStatus(%%):\n") % path);
	output()->write(format("File(%%).access(File::readable) = %%\n") % path % File(path).access(File::Read));
	output()->write(format("File(%%).access(File::writable) = %%\n") % path % File(path).access(File::Write));
	output()->write(format("File(%%).access(File::executable) = %%\n") % path % File(path).access(File::Execute));
	output()->write(format("stat.exists() = %%\n") % stat.exists());
	output()->write(format("stat.mode() = %oct%\n") % stat.mode());
	output()->write(format("stat.size() = %%\n") % stat.size());
	output()->write(format("stat.owner() = \"%%\"\n") % stat.owner());
	output()->write(format("stat.group() = \"%%\"\n") % stat.group());
	output()->write("\n");
}

int main()
{
	printStatus("/usr/include");
	printStatus("testFileStatus");
	printStatus("hmpf.xyz");

	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
