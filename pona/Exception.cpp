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
	
	int lenPath = ::strlen(path);
	int lenLineStr = ::strlen(lineStr);
	int lenClassName = ::strlen(className);
	int lenReason = ::strlen(reason);
	int len = lenPath + ::strlen(":") +
	          lenLineStr + ::strlen(": ") +
	          lenClassName + ::strlen(": ") +
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

