/*
 * Exception.hpp -- exception generators
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_EXCEPTION_HPP
#define FTL_EXCEPTION_HPP

#include <exception>
#include <errno.h> // EBUSY, errno

namespace ftl
{

typedef std::exception AnyException;
typedef std::exception StdException;

class Exception: public AnyException
{
public:
	Exception();
	Exception(const char* path, int line, const char* className, char* reason);
	~Exception() throw();
	const char* what() const throw();
	
protected:
	char* message_;
};

#define FTL_EXCEPTION(DerivedClass, BaseClass) \
class DerivedClass: public BaseClass \
{ \
public: \
	DerivedClass(const char* fileName, int line, const char* className, char* reason) \
		: BaseClass(fileName, line, className, reason) \
	{} \
}

char* captureExceptionMessage(const char* s);
char* captureExceptionMessage(char* s);

#define FTL_THROW(ExceptionClass, reason) \
	throw ExceptionClass(__FILE__, __LINE__, #ExceptionClass, captureExceptionMessage(reason))

/*#define FTL_THROW(ExceptionClass, reason) \
	*((char*)0) = 0; // HACK, to come around trace eaters*/

char* systemError();
char* systemError(int errorCode);
char* pthreadError(const char* callName, int errorCode);

class SystemException: public Exception
{
public:
	SystemException(const char* fileName, int line, const char* className, char* reason, int errorCode)
		: Exception(fileName, line, className, reason),
		  errorCode_(errorCode)
	{}
	int errorCode() const { return errorCode_; }
private:
	int errorCode_;
};

#define FTL_SYSTEM_EXCEPTION \
	throw SystemException(__FILE__, __LINE__, "SystemException", systemError(), errno)

class PthreadException: public SystemException
{
public:
	PthreadException(const char* fileName, int line, const char* className, char* reason, int errorCode)
		: SystemException(fileName, line, className, reason, errorCode)
	{}
};

#define FTL_PTHREAD_EXCEPTION(callName, errorCode) \
	throw PthreadException(__FILE__, __LINE__, "PthreadException", pthreadError(callName, errorCode), errorCode)

FTL_EXCEPTION(DebugException, Exception);

inline void check(bool condition, const char* reason = "") {
	#ifndef NDEBUG
	if (!condition) {
		// char* p = 0; *p = 0; // HACK, to come around trace eaters
		FTL_THROW(DebugException, reason);
	}
	#endif
}

} // namespace ftl

#endif // FTL_EXCEPTION_HPP
