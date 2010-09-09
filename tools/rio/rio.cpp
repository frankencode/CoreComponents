/*
 * rio.cpp -- main program
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */

#include <ftl/stdio>
#include "Options.hpp"
#include "Service.hpp"

namespace rio
{

int main(int argc, char** argv)
{
	options()->read(argc, argv);
	Ref<Service, Owner> service = new Service;
	
	if (options()->client()) {
		service->runClient(1);
	}
	else if (options()->server()) {
		service->runServer(1, options()->backlog());
	}
	else if (options()->help()) {
		print(options()->helpText());
	}
	
	return 0;
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
	catch (ftl::AnyException& ex) {
		ftl::printTo(ftl::error(), "(%%) %%\n", ftl::Path(argv[0]).fileName(), ex.what());
	}
	#endif
	return ret;
}
