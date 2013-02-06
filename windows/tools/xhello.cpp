#include <ftl/XClient.hpp>
#include <ftl/XMessageLogger.hpp>
#include <ftl/Process.hpp>
#include <ftl/PrintDebug.hpp>

int main()
{
	using namespace ftl;
	Ref<XWindow> window = new XWindow(100, 100, 200, 200);
	window->show();
	Ref<XMessageLogger> logger = new XMessageLogger;
	logger->start();
	xClient()->start();
	xClient()->wait();
	logger->wait();
	return 0;
}
