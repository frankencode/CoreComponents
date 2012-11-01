#ifdef MACH_BOOTSTRAP
#include "bootstrap.cpp"
#else
#include <ftl/BuildPlan.hpp>
#include <ftl/GccToolChain.hpp>
#endif

using namespace ftl;

int main(int argc, char **argv)
{
	return BuildPlan::newInstance()->run(GccToolChain::newInstance(), argc, argv);
}
