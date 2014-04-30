/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_USERERROR_H
#define FLUX_USERERROR_H

#include <exception>
#include "String.h"

namespace flux
{

class UserError: public std::exception
{
public:
	~UserError() throw() {}
	const char *what() const throw() { return message(); }
	virtual String message() const = 0;
};

class UsageError: public UserError
{
public:
	UsageError(String message): message_(message) {}
	~UsageError() throw() {}
	virtual String message() const { return message_; }

private:
	String message_;
};

class HelpError: public UserError
{
public:
	~HelpError() throw() {}
	virtual String message() const { return "No help, yet ..."; }
};

class SystemError: public UserError
{
public:
	SystemError(String details = "");
	~SystemError() throw() {}
	virtual String message() const { return message_; }
private:
	String message_;
};

} // namespace flux

#endif // FLUX_USERERROR_H
