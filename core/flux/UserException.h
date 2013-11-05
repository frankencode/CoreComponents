/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_USEREXCEPTION_H
#define FLUX_USEREXCEPTION_H

#include "String.h"

namespace flux
{

class UserException
{
public:
	UserException(String message = ""): message_(message) {}
	String message() const { return message_; }
protected:
	String message_;
};

class HelpException: public UserException
{
public:
	HelpException(): UserException("No help, yet ...") {}
};

} // namespace flux

#endif // FLUX_USEREXCEPTION_H
