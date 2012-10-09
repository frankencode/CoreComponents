#include <ftl/stdio>
#include <ftl/BuildPlan.hpp>
#include <ftl/GccToolChain.hpp>

using namespace ftl;

int main(int argc, char **argv)
{
	return BuildPlan::newInstance(GccToolChain::newInstance())->run(argc, argv);
}
