/*
 * Exception.cpp -- exception generators
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <errno.h>
#include <string.h> // memcpy
#include <assert.h>
#include <stdlib.h> // malloc, free

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
	
	char* buf = (char*)::malloc(n);
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
	
	int lenPath = strlen(path);
	int lenLineStr = strlen(lineStr);
	int lenClassName = strlen(className);
	int lenReason = strlen(reason);
	int len = lenPath + strlen(":") +
	          lenLineStr + strlen(": ") +
	          lenClassName + strlen(": ") +
	          lenReason + 1;
	
	message_ = (char*)::malloc(len);
	int i = 0;
	
	::memcpy(message_, path, lenPath);
	i += lenPath;
	::memcpy(message_ + i, ":", 1);
	i += 1;
	::memcpy(message_ + i, lineStr, lenLineStr);
	i += lenLineStr;
	::memcpy(message_ + i, ": ", 2);
	i += 2;
	::memcpy(message_ + i, className, lenClassName);
	i += lenClassName;
	::memcpy(message_ + i, ": ", 2);
	i += 2;
	::memcpy(message_ + i, reason, lenReason);
	i += lenReason;
	
	message_[i] = 0;
	++i;
	
	assert(i == len);
	
	::free(lineStr);
	::free(reason);
}

Exception::~Exception() throw()
{
	::free(message_);
	message_ = 0;
}

const char* Exception::what() const throw() { return message_; }

char* captureExceptionMessage(const char* s) { return ::strdup(s); }
char* captureExceptionMessage(char* s) { return s; }

char* systemError() { return systemError(errno); }

char* systemError(int errorCode)
{
#ifdef __USE_GNU
	const char* unknown = "Unknown error";
	const int bufSize = 1024; // HACK, save bet
	char buf[bufSize];
	::memcpy(buf, unknown, strlen(unknown) + 1);
	return ::strdup(::strerror_r(errorCode, buf, bufSize));
#else
	const char* unknown = "Unknown error";
	const int bufSize = 1024;
	char* buf = (char*)::malloc(bufSize);
	::memcpy(buf, unknown, strlen(unknown) + 1);
	/*int ret = */::strerror_r(errorCode, buf, bufSize);
	return buf;
#endif
}

char* pthreadError(const char* callName, int errorCode)
{
	int len = 0;
	const char* part1 = callName;               len += strlen(part1);
	const char* part2 = "() failed: ";          len += strlen(part2);
	      char* part3 = systemError(errorCode); len += strlen(part3);
	const char* part4 = " (";                   len += strlen(part4);
	      char* part5 = intToStr(errorCode);    len += strlen(part5);
	const char* part6 = ")";                    len += strlen(part6);
	
	char* msg = (char*)::malloc(len + 1);
	int i = 0;
	::memcpy(msg + i, part1, strlen(part1)); i += strlen(part1);
	::memcpy(msg + i, part2, strlen(part2)); i += strlen(part2);
	::memcpy(msg + i, part3, strlen(part3)); i += strlen(part3);
	::memcpy(msg + i, part4, strlen(part4)); i += strlen(part4);
	::memcpy(msg + i, part5, strlen(part5)); i += strlen(part5);
	::memcpy(msg + i, part6, strlen(part6)); i += strlen(part6);
	msg[i] = 0;
	
	::free(part3);
	::free(part5);
	
	return msg;
}

} // namespace pona
