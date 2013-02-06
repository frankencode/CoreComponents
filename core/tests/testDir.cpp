#include <ftl/PrintDebug.hpp>
#include <ftl/Process.hpp>
#include <ftl/Dir.hpp>

namespace ftl
{

int main(int argc, char **argv)
{
	Ref<Dir> dir = Dir::open(Process::cwd());
	for (string name; dir->read(&name);)
		output()->writeLine(name);
	return 0;
}

} // namespace ftl

int main(int argc, char **argv)
{
	return ftl::main(argc, argv);
}
