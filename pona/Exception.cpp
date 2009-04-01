/*
 * Exception.cpp -- exception generators
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <errno.h>
#include <string.h>
#include "Exception.hpp"

namespace pona
{

char* systemError()
{
#ifdef __USE_GNU
	const char* unknown = "Unknown error";
	const int bufSize = 1024;
	char buf[bufSize];
	::memcpy(buf, unknown, strlen(unknown) + 1);
	return ::strdup(::strerror_r(errno, buf, bufSize));
#else
	const char* unknown = "Unknown error";
	const int bufSize = 1024;
	char* buf = (char*)::malloc(bufSize);
	::memcpy(buf, unknown, strlen(unknown) + 1);
	/*int ret = */::strerror_r(errno, buf, bufSize);
	return buf;
#endif
}

} // namespace pona

