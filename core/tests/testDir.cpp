#include <ftl/stdio>
#include <ftl/process>

namespace ftl
{

int main(int argc, char **argv)
{
	hook<Dir> dir = Dir::open(Process::cwd());
	for (string name; dir->read(&name);)
		output()->writeLine(name);
	return 0;
}

} // namespace ftl

int main(int argc, char **argv)
{
	return ftl::main(argc, argv);
}
