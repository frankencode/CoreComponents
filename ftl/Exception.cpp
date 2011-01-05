/*
 * Exception.cpp -- exception generators
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <errno.h>
#include <string.h> // strerror_r
#include "types.hpp"
#include "strings.hpp"
#include "Exception.hpp"

namespace ftl
{

const char* fileName(const char* path)
{
	char sep = '/';
	
	const char* p = path;
	const char* pSaved = path;
	while (*p)
	{
		if (*p == sep)
			pSaved = p + 1;
		++p;
	}
	
	return pSaved;
}

Exception::Exception()
	: message_(0)
{}

Exception::Exception(const char* path, int line, const char* className, char* reason)
{
	path = fileName(path);
	char* lineStr = intToStr(line);
	message_ = str::cat(path, ":", lineStr, ": ", className, ": ", reason);
	delete[] lineStr;
	delete[] reason;
}

Exception::~Exception() throw()
{
	if (message_) delete[] message_;
	message_ = 0;
}

const char* Exception::what() const throw() { return message_; }

char* captureExceptionMessage(const char* s) { return str::dup(s); }
char* captureExceptionMessage(char* s) { return s; }

char* systemError() { return systemError(errno); }

char* systemError(int errorCode)
{
#ifdef __USE_GNU
	const char* unknown = "Unknown error";
	const int bufSize = 1024; // HACK, save bet
	char buf[bufSize];
	mem::cpy(buf, unknown, str::len(unknown) + 1);
	return str::dup(::strerror_r(errorCode, buf, bufSize));
#else
	const char* unknown = "Unknown error";
	const int bufSize = 1024;
	char* buf = new char[bufSize];
	mem::cpy(buf, unknown, str::len(unknown) + 1);
	/*int ret = */::strerror_r(errorCode, buf, bufSize);
	return buf;
#endif
}

char* pthreadError(const char* callName, int errorCode)
{
	char* errorStr = systemError(errorCode);
	char* errorCodeStr = ftl::intToStr(errorCode);
	char* msg = str::cat(callName, "() failed: ", errorStr, " (", errorCodeStr, ")");
	delete[] errorStr;
	delete[] errorCodeStr;
	return msg;
}

} // namespace ftl
