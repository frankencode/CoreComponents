/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_BASE64_H
#define FKIT_BASE64_H

#include "String.h"

namespace fkit
{

class Base64: public Object
{
public:
	static String encode(const String &source);
	static String decode(const String &source);
};

} // namespace fkit

#endif // FKIT_BASE64_H
