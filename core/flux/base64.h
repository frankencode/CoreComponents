/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_BASE64_H
#define FLUX_BASE64_H

#include "String.h"

namespace flux
{

namespace base64
{

String encode(const String &source);
String decode(const String &source);

class DecodeError {
public:
	~DecodeError() throw() {}
};

class IllegalInputSize4Error: public DecodeError {
public:
	~IllegalInputSize4Error() throw() {}

	virtual String message() const {
		return "Base-64 error: input size need to be a multiple of 4 bytes";
	}
};

class IllegalPaddingError: public DecodeError {
public:
	~IllegalPaddingError() throw() {}

	virtual String message() const {
		return "Base-64 error: unsupported padding";
	}
};

class IllegalCharacterError: public DecodeError {
public:
	~IllegalCharacterError() throw() {}

	virtual String message() const {
		return "Base-64 error: illegal symbol";
	}
};

} // namespace base64

} // namespace flux

#endif // FLUX_BASE64_H
