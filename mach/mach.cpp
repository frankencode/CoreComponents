#include "BuildPlan.hpp"

using namespace mach;

int main(int argc, char **argv)
{
	return BuildPlan::create(argc, argv)->run();
}
