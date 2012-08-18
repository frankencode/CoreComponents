/*
 * Exception.hpp -- exception generators
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
	Exception(const char *path, int line, const char *className, const char *reason, bool reasonOnHeap = false);
	~Exception() throw();
	const char *what() const throw();

protected:
	const char *path_;
	int line_;
	const char *className_;
	mutable const char *reason_;
	bool reasonOnHeap_;
	mutable char *message_;
};

#define FTL_EXCEPTION(DerivedClass, BaseClass) \
class DerivedClass: public BaseClass \
{ \
public: \
	DerivedClass(const char *fileName, int line, const char *className, const char *reason, bool reasonOnHeap) \
		: BaseClass(fileName, line, className, reason, reasonOnHeap) \
	{} \
}

inline bool reasonOnHeap(const char *reason) { return false; }
inline bool reasonOnHeap(char *reason) { return true; }

#define FTL_THROW(ExceptionClass, reason) \
	throw ExceptionClass(__FILE__, __LINE__, #ExceptionClass, reason, reasonOnHeap(reason))

/*#define FTL_THROW(ExceptionClass, reason) \
	*((char *)0) = 0; // HACK, to come around trace eaters*/

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

#define FTL_SYSTEM_EXCEPTION \
	throw SystemException(__FILE__, __LINE__, "SystemException", systemError(), errno)

class PthreadException: public SystemException
{
public:
	PthreadException(const char *fileName, int line, const char *className, char *reason, int errorCode)
		: SystemException(fileName, line, className, reason, errorCode)
	{}
};

#define FTL_PTHREAD_EXCEPTION(callName, errorCode) \
	throw PthreadException(__FILE__, __LINE__, "PthreadException", pthreadError(callName, errorCode), errorCode)

FTL_EXCEPTION(DebugException, Exception);

#define FTL_CHECK(condition, Exception, reason) \
	if (!(condition)) FTL_THROW(Exception, reason)

#ifndef NDEBUG
#define FTL_ASSERT(condition) \
	FTL_CHECK(condition, DebugException, "")
#else
#define FTL_ASSERT(condition);
#endif

#ifndef NDEBUG
#define FTL_ASSERT2(condition, reason) \
	FTL_CHECK(condition, DebugException, reason)
#else
#define FTL_ASSERT2(condition, reason);
#endif

class Interrupt {};
class Timeout {};

#define FTL_STD_EXCEPTION(DerivedClass) \
class DerivedClass: public StdException \
{ \
public: \
	DerivedClass(const String &error): error_(error) {} \
	~DerivedClass() throw() {} \
	inline const char *what() const throw() { return error_; } \
private: \
	String error_; \
};

} // namespace ftl

#endif // FTL_EXCEPTION_HPP
