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

#include "String.h"

namespace flux
{

class UserError
{
public:
	UserError(String message = ""): message_(message) {}
	inline String what() const { return message_; }
protected:
	String message_;
};

class HelpError: public UserError
{
public:
	HelpError(): UserError("No help, yet ...") {}
};

class SystemError: public UserError
{
public:
	SystemError(String details = "");
};

} // namespace flux

#endif // FLUX_USERERROR_H
