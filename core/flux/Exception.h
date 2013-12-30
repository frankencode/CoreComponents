/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_EXCEPTION_H
#define FLUX_EXCEPTION_H

#include <exception>
#include <errno.h> // EBUSY, errno

namespace flux
{

typedef std::exception AnyException;

class Exception: public AnyException
{
public:
	Exception();
	Exception(const char *path, int line, const char *className, const char *reason, bool reasonOnHeap = false);
	~Exception() throw();
	const char *what() const throw();
	const char *message() const throw();

protected:
	const char *path_;
	int line_;
	const char *className_;
	mutable const char *reason_;
	bool reasonOnHeap_;
	mutable char *message_;
};

#define FLUX_EXCEPTION(DerivedClass, BaseClass) \
class DerivedClass: public BaseClass \
{ \
public: \
	DerivedClass(const char *fileName, int line, const char *className, const char *reason, bool reasonOnHeap) \
		: BaseClass(fileName, line, className, reason, reasonOnHeap) \
	{} \
}

inline bool reasonOnHeap(const char *reason) { return false; }
inline bool reasonOnHeap(char *reason) { return true; }

template<class Exception>
inline bool throwException(const char *fileName, int line, const char *exceptionClass, const char *reason, bool reasonOnHeap)
{
	throw Exception(fileName, line, exceptionClass, reason, reasonOnHeap);
	return true;
}

#define FLUX_THROW(ExceptionClass, reason) \
	throwException<ExceptionClass>(__FILE__, __LINE__, #ExceptionClass, reason, reasonOnHeap(reason))

char *systemError();
char *systemError(int errorCode);
char *pthreadError(const char *callName, int errorCode);

class SystemException: public Exception
{
public:
	SystemException(const char *fileName, int line, const char *className, char *reason, int errorCode)
		: Exception(fileName, line, className, reason, true),
		  errorCode_(errorCode)
	{}
	int errorCode() const { return errorCode_; }
private:
	int errorCode_;
};

inline bool throwSystemException(const char *fileName, int line)
{
	throw SystemException(fileName, line, "SystemException", systemError(), errno);
	return true;
}

#define FLUX_SYSTEM_EXCEPTION throwSystemException(__FILE__, __LINE__)

class PthreadException: public SystemException
{
public:
	PthreadException(const char *fileName, int line, const char *className, char *reason, int errorCode)
		: SystemException(fileName, line, className, reason, errorCode)
	{}
};

#define FLUX_PTHREAD_EXCEPTION(callName, errorCode) \
	throw PthreadException(__FILE__, __LINE__, "PthreadException", pthreadError(callName, errorCode), errorCode)

FLUX_EXCEPTION(DebugException, Exception);

#define FLUX_CHECK(condition, Exception, reason) \
	if (!(condition)) FLUX_THROW(Exception, reason)

#ifndef NDEBUG
#define FLUX_ASSERT(condition) \
	FLUX_CHECK(condition, DebugException, "")
#else
#define FLUX_ASSERT(condition);
#endif

#ifndef NDEBUG
#define FLUX_ASSERT2(condition, reason) \
	FLUX_CHECK(condition, DebugException, reason)
#else
#define FLUX_ASSERT2(condition, reason);
#endif

class Interrupt {
public:
	Interrupt();
	inline int signal() const { return signal_; }
	const char *signalName() const;
private:
	int signal_;
};

class Timeout {};

#define FLUX_STD_EXCEPTION(DerivedClass) \
class DerivedClass: public std::exception \
{ \
public: \
	DerivedClass(const String &error): error_(error) {} \
	~DerivedClass() throw() {} \
	inline const char *what() const throw() { return error_; } \
private: \
	String error_; \
};

} // namespace flux

#endif // FLUX_EXCEPTION_H
