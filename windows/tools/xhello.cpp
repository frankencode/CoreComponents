#include <ftl/XClient.hpp>
#include <ftl/XMessageLogger.hpp>
#include <ftl/Process.hpp>
#include <ftl/stdio>

int main()
{
	using namespace ftl;
	Ref<XWindow, Owner> window = new XWindow(100, 100, 200, 200);
	window->show();
	Ref<XMessageLogger, Owner> logger = new XMessageLogger;
	logger->start();
	xClient()->start();
	xClient()->wait();
	logger->wait();
	return 0;
}
