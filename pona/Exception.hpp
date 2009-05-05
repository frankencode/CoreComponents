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

class Exception: public AnyException
{
public:
	Exception(const char* path, int line, const char* className, char* reason);
	~Exception() throw();
	const char* what() const throw();
	
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
