#include <pona/stdio>
#include "Options.hpp"
#include "Service.hpp"

namespace rget
{

using namespace pona;

int main(int argc, char** argv)
{
	int ret = 0;
	
	options()->read(argc, argv);
	Ref<Service, Owner> service = new Service;
	
	if (options()->client_) {
		service->startClient(1);
	}
	else if (options()->server_) {
		service->startServer(1, options()->backlog_);
	}
	else if (options()->help_) {
		print(options()->help());
	}
	else {
		print("Missing parameter, try '%% --help'.\n", options()->execName());
		ret = 1;
	}
	
	return ret;
}

} // namespace rget

int main(int argc, char** argv)
{
	int ret = 1;
	#ifdef NDEBUG
	try {
	#endif
		ret = rget::main(argc, argv);
	#ifdef NDEBUG
	}
	catch (pona::AnyException& ex) {
		pona::printTo(pona::error(), "(%%) %%\n", pona::fileName(argv[0]), ex.what());
	}
	#endif
	return ret;
}
