#include <pona/stdio>

namespace pona
{

void printStatus(String path)
{
	FileStatus stat(path);

	print("FileStatus(%%):\n", path);
	print("File(%%).access(File::readable) = %%\n", path, File(path).access(File::Read));
	print("File(%%).access(File::writable) = %%\n", path, File(path).access(File::Write));
	print("File(%%).access(File::executable) = %%\n", path, File(path).access(File::Execute));
	print("stat.exists() = %%\n", stat.exists());
	print("stat.mode() = %oct%\n", stat.mode());
	print("stat.size() = %%\n", stat.size());
	print("stat.owner() = \"%%\"\n", stat.owner());
	print("stat.group() = \"%%\"\n", stat.group());
	print("\n");
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
