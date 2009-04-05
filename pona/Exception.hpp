/*
 * Exception.hpp -- exception generators
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#ifndef PONA_EXCEPTION_HPP
#define PONA_EXCEPTION_HPP

#include <exception>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

namespace pona
{

typedef std::exception AnyException;

inline const char* fileName(const char* path)
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

inline char* intToStr(int value)
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

class Exception: public AnyException
{
public:
	Exception(const char* path, int line, const char* className, char* reason)
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
	
	~Exception() throw()
	{
		::free(message_);
		message_ = 0;
	}
	
	const char* what() const throw() { return message_; }
	
private:
	char* message_;
};

#define PONA_EXCEPTION(DerivedClass, BaseClass) \
class DerivedClass: public BaseClass \
{ \
public: \
	DerivedClass(const char* fileName, int line, const char* className, char* reason) \
		: BaseClass(fileName, line, className, reason) \
	{} \
}

inline char* captureExceptionMessage(const char* s) { return ::strdup(s); }
inline char* captureExceptionMessage(char* s) { return s; }

#define PONA_THROW(ExceptionClass, reason) \
	throw ExceptionClass(__FILE__, __LINE__, #ExceptionClass, captureExceptionMessage(reason))

char* systemError();
PONA_EXCEPTION(SystemException, Exception);
#define PONA_SYSTEM_EXCEPTION \
	throw SystemException(__FILE__, __LINE__, "SystemException", systemError())

} // namespace pona

#endif // PONA_EXCEPTION_HPP
