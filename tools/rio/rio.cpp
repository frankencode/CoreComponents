/*
 * rio.cpp -- main program
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */

#include <pona/stdio>
#include "Options.hpp"
#include "Service.hpp"

namespace rio
{

int main(int argc, char** argv)
{
	int ret = 0;
	
	options()->read(argc, argv);
	Ref<Service, Owner> service = new Service;
	
	if (options()->client_) {
		service->runClient(1);
	}
	else if (options()->server_) {
		service->runServer(1, options()->backlog_);
	}
	else if (options()->help_) {
		print(options()->help());
	}
	
	return ret;
}

} // namespace rio

int main(int argc, char** argv)
{
	int ret = 1;
	#ifdef NDEBUG
	try {
	#endif
		ret = rio::main(argc, argv);
	#ifdef NDEBUG
	}
	catch (pona::AnyException& ex) {
		pona::printTo(pona::error(), "(%%) %%\n", pona::Path(argv[0]).fileName(), ex.what());
	}
	#endif
	return ret;
}
