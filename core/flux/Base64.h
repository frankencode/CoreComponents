/*
 * Copyright (C) 2007-2013 Frank Mertens.
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

class Base64: public Object
{
public:
	static String encode(const String &source);
	static String decode(const String &source);
};

} // namespace flux

#endif // FLUX_BASE64_H
