/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_ERRORS_H
#define FLUX_ERRORS_H

#include <errno.h>
#include <exception>
#include "String.h"

namespace flux
{

class Error: public std::exception
{
public:
	~Error() throw() {}

	virtual String message() const = 0;

private:
	const char *what() const throw() {
		static String h;
		h = message();
		return h;
	}
};

inline String str(const Error &ex) { return ex.message(); }

class UsageError: public Error
{
public:
	UsageError(String message): message_(message) {}
	~UsageError() throw() {}

	virtual String message() const { return message_; }

private:
	String message_;
};

class HelpError: public Error
{
public:
	~HelpError() throw() {}

	virtual String message() const { return "No help, yet ..."; }
};

class EncodingError: public Error
{
public:
	~EncodingError() throw() {}
};

class UnexpectedEndOfInputError: public Error
{
public:
	~UnexpectedEndOfInputError() throw() {}

	virtual String message() const { return "Unexpected end of input"; }
};

class BufferOverflow: public Error
{
public:
	~BufferOverflow() throw() {}

	virtual String message() const { return "Buffer overflow"; }
};

class DebugError: public Error
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

class SystemError: public Error
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
	const char *source_;
	int line_;
};

class SystemDebugError: public SystemError
{
public:
	SystemDebugError(int errorCode, const char *source, int line)
		: SystemError(errorCode),
		  source_(source),
		  line_(line)
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

class TextError: public Error
{
public:
	inline String text() const { return text_; }
	inline int offset() const { return offset_; }
	inline String resource() const { return resource_; }

	void setResource(String resource) { resource_ = resource; }

protected:
	TextError(String text, int offset, String resource = "");
	~TextError() throw() {}

	String text_;
	int offset_;
	String resource_;
};

class SyntaxState;

class SyntaxError: public TextError
{
public:
	SyntaxError(String text, SyntaxState *state = 0, String resource = "");
	~SyntaxError() throw();

	inline SyntaxState *state() const { return state_; }

	virtual String message() const;

private:
	Ref<SyntaxState> state_;
};

class SemanticError: public TextError
{
public:
	SemanticError(String reason, String text = "", int offset = -1, String resource = "")
		: TextError(text, offset, resource),
		  reason_(reason)
	{}
	~SemanticError() throw() {}

	inline String reason() const { return reason_; }

	virtual String message() const;

private:
	String reason_;
};

class Interrupt: public Error
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

class Timeout: public Error
{
public:
	~Timeout() throw() {}

	virtual String message() const;
};

class ProcessError: public Error
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

#endif // FLUX_ERRORS_H
