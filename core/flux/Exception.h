/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_EXCEPTION_H
#define FLUX_EXCEPTION_H

#include <exception>
#include "String.h"

namespace flux
{

class Exception: public std::exception
{
public:
	~Exception() throw() {}

	virtual String message() const = 0;

private:
	const char *what() const throw() {
		static String h;
		h = message();
		return h;
	}
};

inline String str(const Exception &ex) { return ex.message(); }

} // namespace flux

#endif // FLUX_EXCEPTION_H
