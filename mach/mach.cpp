#include "BuildPlan.hpp"

using namespace ftl;

int main(int argc, char **argv)
{
	return BuildPlan::create(argc, argv)->run();
}
