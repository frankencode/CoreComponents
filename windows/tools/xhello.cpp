#include <ftl/XClient.hpp>
#include <ftl/Process.hpp>
#include <ftl/stdio>

int main()
{
	using namespace ftl;
	Ref<XWindow, Owner> window = xClient()->createWindow(100, 100, 200, 200);
	xClient()->mapWindow(window);
	while (true) Process::sleep(1);
	return 0;
}
