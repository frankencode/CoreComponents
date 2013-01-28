#include <ftl/stdio>
#include <ftl/process>

namespace ftl
{

int main(int argc, char **argv)
{
	O<Dir> dir = Dir::open(Process::cwd());
	for (String name; dir->read(&name);)
		output()->writeLine(name);
	return 0;
}

} // namespace ftl

int main(int argc, char **argv)
{
	return ftl::main(argc, argv);
}
