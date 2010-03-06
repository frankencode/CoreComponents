/*
 * Exception.cpp -- exception generators
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <errno.h>
#include <string.h> // strerror_r
#include "types.hpp"
#include "strings.hpp"
#include "Exception.hpp"

namespace pona
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

char* intToStr(int value)
{
	int n = (value < 0) + 1;
	for (int v = value; v != 0; v /= 10) ++n;
	
	char* buf = new char[n];
	int i = 0;
	if (value < 0)
		buf[0] = '-';
	
	i = n - 1;
	buf[i--] = 0;
	for (int v = value; v != 0; v /= 10)
		buf[i--] = '0' + (v % 10);
	
	return buf;
}

Exception::Exception(const char* path, int line, const char* className, char* reason)
{
	path = fileName(path);
	char* lineStr = intToStr(line);
	message_ = pona::strcat(path, ":", lineStr, ": ", className, ": ", reason);
	delete[] lineStr;
	delete[] reason;
}

Exception::~Exception() throw()
{
	delete[] message_;
	message_ = 0;
}

const char* Exception::what() const throw() { return message_; }

char* captureExceptionMessage(const char* s) { return pona::strdup(s); }
char* captureExceptionMessage(char* s) { return s; }

char* systemError() { return systemError(errno); }

char* systemError(int errorCode)
{
#ifdef __USE_GNU
	const char* unknown = "Unknown error";
	const int bufSize = 1024; // HACK, save bet
	char buf[bufSize];
	pona::memcpy(buf, unknown, pona::strlen(unknown) + 1);
	return pona::strdup(::strerror_r(errorCode, buf, bufSize));
#else
	const char* unknown = "Unknown error";
	const int bufSize = 1024;
	char* buf = new char[bufSize];
	pona::memcpy(buf, unknown, pona::strlen(unknown) + 1);
	/*int ret = */::strerror_r(errorCode, buf, bufSize);
	return buf;
#endif
}

char* pthreadError(const char* callName, int errorCode)
{
	char* errorStr = systemError(errorCode);
	char* errorCodeStr = intToStr(errorCode);
	char* msg = pona::strcat(callName, "() failed: ", errorStr, " (", errorCodeStr, ")");
	delete[] errorStr;
	delete[] errorCodeStr;
	return msg;
}

} // namespace pona
