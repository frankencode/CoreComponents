/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_EXCEPTION_HPP
#define PONA_EXCEPTION_HPP

#ifdef _MSC_VER
#pragma warning ( disable : 4267 4996 )
#endif

#include <exception>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

namespace pona
{

inline const char* fileName(const char* path)
{
#ifdef _WIN32
	char sep = '\\';
#else
	char sep = '/';
#endif
	
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

class Exception: public std::exception
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

#ifdef PONA_POSIX

char* posixError();
PONA_EXCEPTION(PosixException, Exception);
#define PONA_POSIX_EXCEPTION \
	throw PosixException(__FILE__, __LINE__, "PosixException", posixError())

inline char* systemError() { return posixError(); }
#define PONA_SYSTEM_EXCEPTION \
	throw PosixException(__FILE__, __LINE__, "PosixException", posixError())

#else // if PONA_WINDOWS

char* windowsError();
PONA_EXCEPTION(WindowsException, Exception);
#define PONA_WINDOWS_EXCEPTION \
	throw WindowsException(__FILE__, __LINE__, "WindowsException", windowsError())

inline char* systemError() { return windowsError(); }
#define PONA_SYSTEM_EXCEPTION \
	throw WindowsException(__FILE__, __LINE__, "WindowsException", windowsError())

#endif

} // namespace pona

#endif // PONA_EXCEPTION_HPP
