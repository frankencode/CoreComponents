#include <ftl/XClient.hpp>
#include <ftl/Process.hpp>
#include <ftl/stdio>

int main()
{
	using namespace ftl;
	Ref<XWindow, Owner> window = xClient()->createWindow(100, 100, 200, 200);
	xClient()->mapWindow(window);
	xClient()->start();
	xClient()->wait();
	return 0;
}
