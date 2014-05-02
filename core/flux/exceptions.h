/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_EXCEPTIONS_H
#define FLUX_EXCEPTIONS_H

#include <errno.h>
#include <exception>
#include "String.h"

namespace flux
{

class Exception: public std::exception
{
public:
	~Exception() throw() {}
	const char *what() const throw() { return message(); }

	virtual String message() const = 0;
};

class UsageError: public Exception
{
public:
	UsageError(String message): message_(message) {}
	~UsageError() throw() {}

	virtual String message() const { return message_; }

private:
	String message_;
};

class HelpError: public Exception
{
public:
	~HelpError() throw() {}

	virtual String message() const { return "No help, yet ..."; }
};

class DebugError: public Exception
{
public:
	DebugError(String reason, const char *source, int line)
		: reason_(reason),
		  source_(source),
		  line_(line)
	{}
	~DebugError() throw() {}

	virtual String message() const;

private:
	String reason_;
	const char *source_;
	int line_;
};

class SystemError: public Exception
{
public:
	SystemError(int errorCode): errorCode_(errorCode) {}
	~SystemError() throw() {}

	inline int errorCode() const { return errorCode_; }

protected:
	int errorCode_;
};

class SystemResourceError: public SystemError
{
public:
	SystemResourceError(int errorCode, String resource, const char *source, int line)
		: SystemError(errorCode),
		  resource_(resource),
		  source_(source),
		  line_(line)
	{}
	~SystemResourceError() throw() {}

	inline String resource() const { return resource_; }
	inline const char *source() const { return source_; }
	inline int line() const { return line_; }

	virtual String message() const;

private:
	String resource_;
	String source_;
	int line_;
};

class SystemDebugError: public SystemError
{
public:
	SystemDebugError(int errorCode, const char *source, int line)
		: SystemError(errorCode)
	{}
	~SystemDebugError() throw() {}

	inline const char *source() const { return source_; }
	inline int line() const { return line_; }

	virtual String message() const;

private:
	const char *source_;
	int line_;
};

#define FLUX_DEBUG_ERROR(reason) \
	throw DebugError(reason, __FILE__, __LINE__)

#define FLUX_SYSTEM_RESOURCE_ERROR(errorCode, resource) \
	throw SystemResourceError(errorCode, resource, __FILE__, __LINE__)

#define FLUX_SYSTEM_DEBUG_ERROR(errorCode) \
	throw SystemDebugError(errorCode, __FILE__, __LINE__)

#define FLUX_SYSTEM_ERROR(errorCode, resource) \
	{ \
		if (resource != "") FLUX_SYSTEM_RESOURCE_ERROR(errorCode, resource); \
		else FLUX_SYSTEM_DEBUG_ERROR(errorCode); \
	}

class Interrupt: public Exception
{
public:
	Interrupt();
	~Interrupt() throw() {}

	inline int signal() const { return signal_; }
	const char *signalName() const;

	virtual String message() const;

private:
	int signal_;
};

class Timeout: public Exception
{
public:
	~Timeout() throw() {}

	virtual String message() const;
};

class ProcessError: public Exception
{
public:
	ProcessError(int status, String command)
		: status_(status),
		  command_(command)
	{}
	~ProcessError() throw() {}

	inline int status() const { return status_; }
	inline String command() const { return command_; }

	virtual String message() const;

private:
	int status_;
	String command_;
};

} // namespace flux

#endif // FLUX_EXCEPTIONS_H
