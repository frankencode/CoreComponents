/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_EXCEPTION_H
#define FKIT_EXCEPTION_H

#include <exception>
#include <errno.h> // EBUSY, errno

namespace fkit
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

#define FKIT_EXCEPTION(DerivedClass, BaseClass) \
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

#define FKIT_THROW(ExceptionClass, reason) \
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

#define FKIT_SYSTEM_EXCEPTION throwSystemException(__FILE__, __LINE__)

class PthreadException: public SystemException
{
public:
	PthreadException(const char *fileName, int line, const char *className, char *reason, int errorCode)
		: SystemException(fileName, line, className, reason, errorCode)
	{}
};

#define FKIT_PTHREAD_EXCEPTION(callName, errorCode) \
	throw PthreadException(__FILE__, __LINE__, "PthreadException", pthreadError(callName, errorCode), errorCode)

FKIT_EXCEPTION(DebugException, Exception);

#define FKIT_CHECK(condition, Exception, reason) \
	if (!(condition)) FKIT_THROW(Exception, reason)

#ifndef NDEBUG
#define FKIT_ASSERT(condition) \
	FKIT_CHECK(condition, DebugException, "")
#else
#define FKIT_ASSERT(condition);
#endif

#ifndef NDEBUG
#define FKIT_ASSERT2(condition, reason) \
	FKIT_CHECK(condition, DebugException, reason)
#else
#define FKIT_ASSERT2(condition, reason);
#endif

class Interrupt {
public:
	Interrupt();
	inline int signal() const { return signal_; }
private:
	int signal_;
};

class Timeout {};

#define FKIT_STD_EXCEPTION(DerivedClass) \
class DerivedClass: public std::exception \
{ \
public: \
	DerivedClass(const String &error): error_(error) {} \
	~DerivedClass() throw() {} \
	inline const char *what() const throw() { return error_; } \
private: \
	String error_; \
};

} // namespace fkit

#endif // FKIT_EXCEPTION_H
