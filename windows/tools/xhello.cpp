#include <ftl/XClient.hpp>
#include <ftl/Process.hpp>
#include <ftl/stdio>

int main()
{
	using namespace ftl;
	Ref<XWindow, Owner> window = new XWindow(100, 100, 200, 200);
	xClient()->createWindow(window);
	xClient()->mapWindow(window);
	xClient()->start();
	xClient()->wait();
	return 0;
}
